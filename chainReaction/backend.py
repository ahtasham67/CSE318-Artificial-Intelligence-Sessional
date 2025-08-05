import copy
import math
from collections import deque

GRID_WIDTH = 9
GRID_HEIGHT = 6

BASIC_ORB_HEURISTIC = 0
WEIGHTED_ORB_HEURISTIC = 1
CELL_CONTROL_HEURISTIC = 2
EVALUATE_STABILITY = 3
EVALUATE_CLUSTER_BONUS = 4

CRITICAL_MASS = {}
CRITICAL_MASS[(0, 0)] = 2
CRITICAL_MASS[(0, GRID_WIDTH - 1)] = 2
CRITICAL_MASS[(GRID_HEIGHT - 1, 0)] = 2
CRITICAL_MASS[(GRID_HEIGHT - 1, GRID_WIDTH - 1)] = 2

for y in range(1, GRID_WIDTH - 1):
    CRITICAL_MASS[(0, y)] = 3
    CRITICAL_MASS[(GRID_HEIGHT - 1, y)] = 3
for x in range(1, GRID_HEIGHT - 1):
    CRITICAL_MASS[(x, 0)] = 3
    CRITICAL_MASS[(x, GRID_WIDTH - 1)] = 3

for x in range(1, GRID_HEIGHT - 1):
    for y in range(1, GRID_WIDTH - 1):
        CRITICAL_MASS[(x, y)] = 4


class Cell:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.orbs = 0
        self.owner = 0  # 0 for no owner, 1 for player 1, 2 for player 2

    def add_orb_from_explosion(self, player_id_of_exploding_cell):
        if self.owner == 0:
            self.owner = player_id_of_exploding_cell
            self.orbs += 1
        elif self.owner == player_id_of_exploding_cell:
            self.orbs += 1
        elif self.owner != player_id_of_exploding_cell:
            self.owner = player_id_of_exploding_cell
            self.orbs += 1

    def explode(self):
        self.orbs = 0
        self.owner = 0 

    def get_critical_mass(self):
        return CRITICAL_MASS.get((self.x, self.y), 4)


class Board:
    def __init__(self):
        self.grid = [[Cell(r, c) for c in range(GRID_WIDTH)] for r in range(GRID_HEIGHT)]
        self.current_player = 1
        self.game_over = False
        self.winner = 0
        self.explosion_in_progress = False  # Flag to manage explosion animations/state
        self.player_has_made_first_move = {1: False, 2: False}  # Track if a player has made their very first move

    def reset(self):
        self.grid = [[Cell(r, c) for c in range(GRID_WIDTH)] for r in range(GRID_HEIGHT)]
        self.current_player = 1
        self.game_over = False
        self.winner = 0
        self.explosion_in_progress = False
        self.player_has_made_first_move = {1: False, 2: False}

    def get_neighbors(self, r, c):
        neighbors = []
        if r > 0:
            neighbors.append(self.grid[r - 1][c])
        if r < GRID_HEIGHT - 1:
            neighbors.append(self.grid[r + 1][c])
        if c > 0:
            neighbors.append(self.grid[r][c - 1])
        if c < GRID_WIDTH - 1:
            neighbors.append(self.grid[r][c + 1])
        return neighbors

    def make_move(self, r, c):
        if self.game_over or self.explosion_in_progress:
            return False

        cell = self.grid[r][c]
        if cell.owner == 0 or cell.owner == self.current_player:
            player_making_move = self.current_player

            if cell.owner == 0:
                cell.owner = player_making_move
            cell.orbs += 1
            self.player_has_made_first_move[player_making_move] = True

            if cell.orbs >= cell.get_critical_mass():
                self.explosion_in_progress = True
                self._handle_explosions()
                self.explosion_in_progress = False

            self._check_game_over(player_making_move)
            return True
        return False

    def _handle_explosions(self):
        explosion_queue = deque()
        cells_in_queue = set()

        for r in range(GRID_HEIGHT):
            for c in range(GRID_WIDTH):
                cell = self.grid[r][c]
                if cell.orbs >= cell.get_critical_mass() and cell.owner != 0:
                    explosion_queue.append((r, c, cell.owner))
                    cells_in_queue.add((r, c))

        while explosion_queue:
            r, c, player_id_who_caused_this_explosion = explosion_queue.popleft()
            if (r, c) in cells_in_queue:
                cells_in_queue.remove((r, c))

            cell = self.grid[r][c]
            if cell.orbs < cell.get_critical_mass() or cell.owner == 0:
                continue
            owner_of_cell_about_to_explode = cell.owner
            cell.explode()

            neighbors = self.get_neighbors(r, c)
            for neighbor in neighbors:
                neighbor.add_orb_from_explosion(owner_of_cell_about_to_explode)

                if neighbor.orbs >= neighbor.get_critical_mass() and (neighbor.x, neighbor.y) not in cells_in_queue:
                    explosion_queue.append((neighbor.x, neighbor.y, neighbor.owner))
                    cells_in_queue.add((neighbor.x, neighbor.y))

    def _check_game_over(self, player_who_just_moved):
        if self.game_over:
            return

        opponent = 3 - player_who_just_moved
        player_orbs = 0
        opponent_orbs = 0

        for r in range(GRID_HEIGHT):
            for c in range(GRID_WIDTH):
                cell = self.grid[r][c]
                if cell.owner == player_who_just_moved:
                    player_orbs += cell.orbs
                elif cell.owner == opponent:
                    opponent_orbs += cell.orbs

        if opponent_orbs == 0 and player_orbs > 0 and self.player_has_made_first_move[opponent]:
            self.game_over = True
            self.winner = player_who_just_moved

    def to_file_format(self, player_id_who_is_about_to_move):
        header_str = ""
        if player_id_who_is_about_to_move == 1:
            header_str = "AI Move:"
        elif player_id_who_is_about_to_move == 2:
            header_str = "Human Move:"
        else:
            header_str = "Invalid Move:"

        board_lines = []
        for r in range(GRID_HEIGHT):
            row_cells = []
            for c in range(GRID_WIDTH):
                cell = self.grid[r][c]
                if cell.owner == 0:
                    row_cells.append("0")
                else:
                    orb_char = 'B' if cell.owner == 1 else 'R'
                    row_cells.append(f"{cell.orbs}{orb_char}")
            board_lines.append(" ".join(row_cells))
        return header_str + "\n" + "\n".join(board_lines)

    def from_file_format(self, file_content):
        lines = file_content.strip().split('\n')
        if not lines:
            raise ValueError("File content is empty or malformed.")

        header = lines[0].strip()

        if header == "Human Move:":
            self.current_player = 2
        elif header == "AI Move:":
            self.current_player = 1
        else:
            raise ValueError(f"Invalid header in gamestate file: {header}")

        self.grid = [[Cell(r, c) for c in range(GRID_WIDTH)] for r in range(GRID_HEIGHT)]

        for r_idx, line in enumerate(lines[1:]):
            cells_str = line.split(' ')
            if len(cells_str) != GRID_WIDTH:
                raise ValueError(f"Malformed row in gamestate file: {line}")
            for c_idx, cell_str in enumerate(cells_str):
                cell = self.grid[r_idx][c_idx]
                if cell_str == "0":
                    cell.orbs = 0
                    cell.owner = 0
                else:
                    orb_count = int(cell_str[:-1])
                    orb_char = cell_str[-1]
                    cell.orbs = orb_count
                    cell.owner = 1 if orb_char == 'B' else 2

        self.player_has_made_first_move = {1: False, 2: False}
        player1_orbs = 0
        player2_orbs = 0
        for r in range(GRID_HEIGHT):
            for c in range(GRID_WIDTH):
                if self.grid[r][c].owner == 1:
                    player1_orbs += self.grid[r][c].orbs
                elif self.grid[r][c].owner == 2:
                    player2_orbs += self.grid[r][c].orbs

        if player1_orbs > 0:
            self.player_has_made_first_move[1] = True
        if player2_orbs > 0:
            self.player_has_made_first_move[2] = True

        return header


class AI:
    def __init__(self, player_id, heuristic_type=BASIC_ORB_HEURISTIC):
        self.player_id = player_id
        self.opponent_id = 3 - player_id
        self.MAX_DEPTH = 1
        self.heuristic_type = heuristic_type

    def get_possible_moves(self, board_state):
        moves = []
        for r in range(GRID_HEIGHT):
            for c in range(GRID_WIDTH):
                cell = board_state.grid[r][c]
                if cell.owner == 0 or cell.owner == board_state.current_player:
                    moves.append((r, c))
        return moves

    def _evaluate_basic_orb_diff(self, board_state):
        player_orbs = 0
        opponent_orbs = 0
        for r in range(GRID_HEIGHT):
            for c in range(GRID_WIDTH):
                cell = board_state.grid[r][c]
                if cell.owner == self.player_id:
                    player_orbs += cell.orbs
                elif cell.owner == self.opponent_id:
                    opponent_orbs += cell.orbs
        score = player_orbs - opponent_orbs
        return score

    def _evaluate_weighted_orb_and_imminent_explosions(self, board_state):
        score = 0
        for r in range(GRID_HEIGHT):
            for c in range(GRID_WIDTH):
                cell = board_state.grid[r][c]
                cell_critical_mass = cell.get_critical_mass()

                if cell.owner == self.player_id:
                    score += cell.orbs
                    if cell.orbs >= cell_critical_mass - 1:
                        score += 15
                elif cell.owner == self.opponent_id:
                    score -= cell.orbs
                    if cell.orbs >= cell_critical_mass - 1:
                        score -= 15
        return score

    def _evaluate_cell_control(self, board_state):
        score = 0
        player_owned_cells_count = 0
        opponent_owned_cells_count = 0

        for r in range(GRID_HEIGHT):
            for c in range(GRID_WIDTH):
                cell = board_state.grid[r][c]
                cell_critical_mass = cell.get_critical_mass()

                if cell.owner == self.player_id:
                    player_owned_cells_count += 1
                    score += cell.orbs
                    if cell_critical_mass == 2:
                        score += 5
                    elif cell_critical_mass == 3:
                        score += 3

                    if cell.orbs >= cell_critical_mass - 1:
                        for neighbor in board_state.get_neighbors(r, c):
                            if neighbor.owner == self.opponent_id:
                                score += 20

                elif cell.owner == self.opponent_id:
                    opponent_owned_cells_count += 1
                    score -= cell.orbs
                    if cell_critical_mass == 2:
                        score -= 5
                    elif cell_critical_mass == 3:
                        score -= 3

        score += (player_owned_cells_count - opponent_owned_cells_count) * 10
        return score

    def _evaluate_stability(self, board_state):
        score = 0
        for r in range(GRID_HEIGHT):
            for c in range(GRID_WIDTH):
                cell = board_state.grid[r][c]
                critical = cell.get_critical_mass()

                if cell.owner == self.player_id:
                    distance_to_blast = critical - cell.orbs
                    score += distance_to_blast * 2
                elif cell.owner == self.opponent_id:
                    distance_to_blast = critical - cell.orbs
                    score -= distance_to_blast * 2
        return score

    def _evaluate_cluster_bonus(self, board_state):
        score = 0
        for r in range(GRID_HEIGHT):
            for c in range(GRID_WIDTH):
                cell = board_state.grid[r][c]
                if cell.owner == self.player_id:
                    same_owner_neighbors = sum(1 for n in board_state.get_neighbors(r, c) if n.owner == self.player_id)
                    score += same_owner_neighbors * 5
                elif cell.owner == self.opponent_id:
                    same_owner_neighbors = sum(1 for n in board_state.get_neighbors(r, c) if n.owner == self.opponent_id)
                    score -= same_owner_neighbors * 5
        return score

    def evaluate_board(self, board_state):
        if board_state.game_over:
            if board_state.winner == self.player_id:
                return float('inf')
            elif board_state.winner == self.opponent_id:
                return float('-inf')

        if self.heuristic_type == BASIC_ORB_HEURISTIC:
            return self._evaluate_basic_orb_diff(board_state)
        elif self.heuristic_type == WEIGHTED_ORB_HEURISTIC:
            return self._evaluate_weighted_orb_and_imminent_explosions(board_state)
        elif self.heuristic_type == CELL_CONTROL_HEURISTIC:
            return self._evaluate_cell_control(board_state)
        elif self.heuristic_type == EVALUATE_STABILITY:
            return self._evaluate_stability(board_state)
        elif self.heuristic_type == EVALUATE_CLUSTER_BONUS:
            return self._evaluate_cluster_bonus(board_state)
        else:
            raise ValueError("Unknown heuristic type")

    def minimax(self, board_state, depth, alpha, beta, maximizing_player):
        if depth == self.MAX_DEPTH or board_state.game_over:
            return self.evaluate_board(board_state)

        possible_moves = self.get_possible_moves(board_state)
        if not possible_moves:
            return self.evaluate_board(board_state)

        if maximizing_player:
            max_eval = float('-inf')
            for move in possible_moves:
                new_board = copy.deepcopy(board_state)
                new_board.current_player = self.player_id
                if new_board.make_move(move[0], move[1]):
                    new_board.current_player = self.opponent_id
                    eval = self.minimax(new_board, depth + 1, alpha, beta, False)
                    max_eval = max(max_eval, eval)
                    alpha = max(alpha, eval)
                    if beta <= alpha:
                        break
            return max_eval
        else:
            min_eval = float('inf')
            for move in possible_moves:
                new_board = copy.deepcopy(board_state)
                new_board.current_player = self.opponent_id
                if new_board.make_move(move[0], move[1]):
                    new_board.current_player = self.player_id
                    eval = self.minimax(new_board, depth + 1, alpha, beta, True)
                    min_eval = min(min_eval, eval)
                    beta = min(beta, eval)
                    if beta <= alpha:
                        break
            return min_eval

    def get_best_move(self, board_state, maximizing):
        best_move = None
        best_eval = float('-inf') if maximizing else float('inf')
        possible_moves = self.get_possible_moves(board_state)
        # random.shuffle(possible_moves) 
        for move in possible_moves:
            new_board = copy.deepcopy(board_state)
            new_board.current_player = self.player_id

            if new_board.make_move(move[0], move[1]):
                new_board.current_player = self.opponent_id
                eval = self.minimax(new_board, 0, float('-inf'), float('inf'), not maximizing)
                if maximizing:
                    if eval > best_eval:
                        best_eval = eval
                        best_move = move
                else:
                    if eval < best_eval:
                        best_eval = eval
                        best_move = move
        print(best_eval)
        print(best_move) 
        return best_move
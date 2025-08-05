import os
import sys
import time

import pygame

from backend import (AI, BASIC_ORB_HEURISTIC, CELL_CONTROL_HEURISTIC,
                     EVALUATE_CLUSTER_BONUS, EVALUATE_STABILITY, GRID_HEIGHT,
                     GRID_WIDTH, WEIGHTED_ORB_HEURISTIC, Board)

CELL_SIZE = 60
TOOLBAR_HEIGHT = 80
WINDOW_WIDTH = GRID_WIDTH * CELL_SIZE
WINDOW_HEIGHT = GRID_HEIGHT * CELL_SIZE + TOOLBAR_HEIGHT

WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (255, 0, 0)
BLUE = (0, 0, 255)
GREEN = (0, 255, 0)
YELLOW = (255, 255, 0)
GRAY = (200, 200, 200)

MENU = 0
GAME_PLAYING = 1
GAME_OVER = 2

HUMAN_VS_HUMAN = 0
HUMAN_VS_AI = 1
AI_VS_AI = 2


ORB_RADIUS = CELL_SIZE // 3
ORB_COLORS = {1: BLUE, 2: RED}
PLAYER_NAMES = {1: "Blue", 2: "Red"}

GAME_STATE_FILE = "gamestate.txt"
FILE_READ_POLLING_INTERVAL = 0.1

pygame.init()
SCREEN = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
pygame.display.set_caption("Chain Reaction")
FONT = pygame.font.Font(None, 36)
SMALL_FONT = pygame.font.Font(None, 24)
CLOCK = pygame.time.Clock()


class FrontendCellRenderer:
    """Handles drawing a single cell based on its backend data."""
    def __init__(self, backend_cell):
        self.backend_cell = backend_cell

    def draw(self, screen):
        rect = pygame.Rect(self.backend_cell.y * CELL_SIZE, self.backend_cell.x * CELL_SIZE + TOOLBAR_HEIGHT, CELL_SIZE, CELL_SIZE)
        pygame.draw.rect(screen, GRAY, rect, 1)

        if self.backend_cell.orbs > 0:
            orb_color = ORB_COLORS.get(self.backend_cell.owner, BLACK)
            center_x = rect.centerx
            center_y = rect.centery

            if self.backend_cell.orbs == 1:
                pygame.draw.circle(screen, orb_color, (center_x, center_y), ORB_RADIUS)
            elif self.backend_cell.orbs == 2:
                pygame.draw.circle(screen, orb_color, (center_x - ORB_RADIUS // 2, center_y), ORB_RADIUS)
                pygame.draw.circle(screen, orb_color, (center_x + ORB_RADIUS // 2, center_y), ORB_RADIUS)
            elif self.backend_cell.orbs == 3:
                pygame.draw.circle(screen, orb_color, (center_x, center_y - ORB_RADIUS // 2), ORB_RADIUS)
                pygame.draw.circle(screen, orb_color, (center_x - ORB_RADIUS // 2, center_y + ORB_RADIUS // 2), ORB_RADIUS)
                pygame.draw.circle(screen, orb_color, (center_x + ORB_RADIUS // 2, center_y + ORB_RADIUS // 2), ORB_RADIUS)
            elif self.backend_cell.orbs >= 4:
                offset = ORB_RADIUS // 2
                pygame.draw.circle(screen, orb_color, (center_x - offset, center_y - offset), ORB_RADIUS)
                pygame.draw.circle(screen, orb_color, (center_x + offset, center_y - offset), ORB_RADIUS)
                pygame.draw.circle(screen, orb_color, (center_x - offset, center_y + offset), ORB_RADIUS)
                pygame.draw.circle(screen, orb_color, (center_x + offset, center_y + offset), ORB_RADIUS)

                if self.backend_cell.orbs > 4:
                    text_surface = SMALL_FONT.render(f"+{self.backend_cell.orbs - 4}", True, BLACK)
                    text_rect = text_surface.get_rect(center=(center_x, center_y))
                    screen.blit(text_surface, text_rect)


class GameFrontend:
    def __init__(self):
        self.state = MENU
        self.game_mode = HUMAN_VS_AI
        self.board = Board()  # The backend board instance
        self.player1_ai = AI(1, heuristic_type=WEIGHTED_ORB_HEURISTIC)
        self.player2_ai = AI(2, heuristic_type=CELL_CONTROL_HEURISTIC)
        self.ai_delay_ms = 50
        self.game_over_processed = False
        self.expected_file_header = "Human Move:"

        self.start_time = 0.0
        self.end_time = 0.0
        self.match_duration = 0.0

    def handle_menu_input(self, event):
        if event.type == pygame.MOUSEBUTTONDOWN:
            mouse_x, mouse_y = event.pos
            if 200 <= mouse_x <= 400 and 200 <= mouse_y <= 250:
                self.game_mode = HUMAN_VS_HUMAN
                self.start_game()
            elif 200 <= mouse_x <= 400 and 260 <= mouse_y <= 310:
                self.game_mode = HUMAN_VS_AI
                self.start_game()
            elif 200 <= mouse_x <= 400 and 320 <= mouse_y <= 370:
                self.game_mode = AI_VS_AI
                self.start_game()

    def handle_game_playing_input(self, event):
        if self.board.game_over or self.board.explosion_in_progress:
            return

        current_file_header = self._read_game_state_from_file(self.expected_file_header)
        if current_file_header is None:
            return

        current_player = self.board.current_player
        is_human_controlled_player = False
        if self.game_mode == HUMAN_VS_HUMAN:
            is_human_controlled_player = True
        elif self.game_mode == HUMAN_VS_AI and current_player == 1:
            is_human_controlled_player = True

        if is_human_controlled_player:
            if event.type == pygame.MOUSEBUTTONDOWN:
                mouse_x, mouse_y = event.pos
                if mouse_y > TOOLBAR_HEIGHT:
                    grid_x = (mouse_y - TOOLBAR_HEIGHT) // CELL_SIZE
                    grid_y = mouse_x // CELL_SIZE
                    if 0 <= grid_x < GRID_HEIGHT and 0 <= grid_y < GRID_WIDTH:
                        if self.board.make_move(grid_x, grid_y):
                            player_who_just_moved = current_player
                            next_player_id = 3 - player_who_just_moved
                            self._write_game_state_to_file(next_player_id)
                            self.expected_file_header = "AI Move:" if next_player_id == 1 else "Human Move:"
                            self.board.current_player = next_player_id
                            print(f"Player {player_who_just_moved} (Human control) made move and wrote to file. Expecting '{self.expected_file_header}'")

    def handle_game_over_input(self, event):
        if event.type == pygame.MOUSEBUTTONDOWN:
            mouse_x, mouse_y = event.pos
            if WINDOW_WIDTH // 2 - 75 <= mouse_x <= WINDOW_WIDTH // 2 + 75 and WINDOW_HEIGHT // 2 + 50 <= mouse_y <= WINDOW_HEIGHT // 2 + 100:
                self.state = MENU
                self.board.reset()
                if os.path.exists(GAME_STATE_FILE):
                    try:
                        os.remove(GAME_STATE_FILE)
                        print(f"Removed {GAME_STATE_FILE}")
                    except OSError as e:
                        print(f"Error removing {GAME_STATE_FILE}: {e}")
                self.expected_file_header = "Human Move:"
                self.start_time = 0.0 
                self.end_time = 0.0
                self.match_duration = 0.0
            elif WINDOW_WIDTH // 2 - 75 <= mouse_x <= WINDOW_WIDTH // 2 + 150 and WINDOW_HEIGHT // 2 + 110 <= mouse_y <= WINDOW_HEIGHT // 2 + 160:
                return True

    def start_game(self):
        self.board.reset()
        self.game_over_processed = False
        self._write_game_state_to_file(1)
        self.expected_file_header = "AI Move:"
        print(f"Game started. Initializing {GAME_STATE_FILE} with header 'AI Move:' for Human (Player 1) to read.")
        self.state = GAME_PLAYING
        self.start_time = time.time() 

    def _read_game_state_from_file(self, expected_header_str):
        try:
            if not os.path.exists(GAME_STATE_FILE):
                return None

            with open(GAME_STATE_FILE, 'r') as f:
                content = f.read()

            if not content.strip():
                return None

            actual_header = content.split('\n')[0].strip()

            if actual_header == expected_header_str:
                if not self.board.game_over:
                    self.board.from_file_format(content)
                    return actual_header
                else:
                    print(f"Skipping board update from file '{actual_header}' as game is already internally marked as over.")
                    return actual_header
            else:
                pass
        except (IOError, ValueError) as e:
            print(f"Error reading/parsing {GAME_STATE_FILE}: {e}")
            pass
        return None

    def _write_game_state_to_file(self, next_player_id):
        if self.board.game_over:
            print(f"Attempted to write to file, but game is over. Skipping write.")
            return

        try:
            content = self.board.to_file_format(next_player_id)
            with open(GAME_STATE_FILE, 'w') as f:
                f.write(content)
            print(f"Wrote game state to {GAME_STATE_FILE}. File Header: {content.splitlines()[0].strip()}")
        except IOError as e:
            print(f"Error writing to {GAME_STATE_FILE}: {e}")

    def update(self):
        if self.state == GAME_OVER and self.game_over_processed:
            return
        if self.board.game_over:
            if not self.game_over_processed:
                self.state = GAME_OVER
                self.game_over_processed = True
                self.end_time = time.time() 
                self.match_duration = self.end_time - self.start_time
                print(f"Game over detected! Winner: Player {self.board.winner}. Match Duration: {self.match_duration:.2f} seconds. Transitioning to GAME_OVER state.")
                if os.path.exists(GAME_STATE_FILE):
                    try:
                        os.remove(GAME_STATE_FILE)
                        print(f"CLEANED: Removed {GAME_STATE_FILE} due to game over.")
                    except OSError as e:
                        print(f"Error removing {GAME_STATE_FILE} during game over cleanup: {e}")
            return
        if self.board.explosion_in_progress:
            return

        current_file_header = self._read_game_state_from_file(self.expected_file_header)
        if current_file_header is None:
            time.sleep(FILE_READ_POLLING_INTERVAL)
            return

        if self.board.game_over:
            if not self.game_over_processed:
                self.state = GAME_OVER
                self.game_over_processed = True
                self.end_time = time.time() 
                self.match_duration = self.end_time - self.start_time
                print(f"Game over detected AFTER FILE READ! Winner: Player {self.board.winner}. Match Duration: {self.match_duration:.2f} seconds. Transitioning to GAME_OVER state.")
                if os.path.exists(GAME_STATE_FILE):
                    try:
                        os.remove(GAME_STATE_FILE)
                        print(f"CLEANED: Removed {GAME_STATE_FILE} due to game over (after file read).")
                    except OSError as e:
                        print(f"Error removing {GAME_STATE_FILE} during game over cleanup: {e}")
            return

        current_player = self.board.current_player
        is_ai_controlled_player = False
        if self.game_mode == HUMAN_VS_AI and current_player == 2:
            is_ai_controlled_player = True
        elif self.game_mode == AI_VS_AI:
            is_ai_controlled_player = True

        if is_ai_controlled_player:
            print(f"AI (Player {current_player}) is calculating its move...")
            pygame.time.wait(self.ai_delay_ms)
            ai_to_use = self.player1_ai if current_player == 1 else self.player2_ai
            maximizing = True if current_player == 1 else False
            best_move = ai_to_use.get_best_move(self.board, maximizing)
            print(f"AI (Player {current_player}) chose move: {best_move}")

            if best_move:
                if self.board.make_move(best_move[0], best_move[1]):
                    if self.board.game_over:
                        if not self.game_over_processed:
                            self.state = GAME_OVER
                            self.game_over_processed = True
                            self.end_time = time.time()
                            self.match_duration = self.end_time - self.start_time
                            print(f"Game over detected AFTER AI MOVE! Winner: Player {self.board.winner}. Match Duration: {self.match_duration:.2f} seconds. Transitioning to GAME_OVER state.")
                            if os.path.exists(GAME_STATE_FILE):
                                try:
                                    os.remove(GAME_STATE_FILE)
                                    print(f"CLEANED: Removed {GAME_STATE_FILE} due to game over (after AI move).")
                                except OSError as e:
                                    print(f"Error removing {GAME_STATE_FILE} during game over cleanup: {e}")
                        return
                    else:
                        next_player_id = 3 - current_player
                        self._write_game_state_to_file(next_player_id)
                        self.expected_file_header = "AI Move:" if next_player_id == 1 else "Human Move:"
                        self.board.current_player = next_player_id
                        print(f"AI (Player {current_player}) made move and wrote to file. Expecting '{self.expected_file_header}'")
            else:
                print(f"AI (Player {current_player}) could not find a valid move. This shouldn't happen unless draw.")
                self.board.game_over = True
                self.board.winner = 0
                if not self.game_over_processed:
                    self.state = GAME_OVER
                    self.game_over_processed = True
                    self.end_time = time.time() 
                    self.match_duration = self.end_time - self.start_time
                    print(f"Game over detected due to AI error/draw! Match Duration: {self.match_duration:.2f} seconds. Transitioning to GAME_OVER state.")
                    if os.path.exists(GAME_STATE_FILE):
                        try:
                            os.remove(GAME_STATE_FILE)
                            print(f"CLEANED: Removed {GAME_STATE_FILE} due to AI error/draw.")
                        except OSError as e:
                            print(f"Error removing {GAME_STATE_FILE} during game over cleanup: {e}")
                return

    def draw_menu(self):
        SCREEN.fill(BLACK)
        title_text = FONT.render("Chain Reaction", True, WHITE)
        title_rect = title_text.get_rect(center=(WINDOW_WIDTH // 2, 100))
        SCREEN.blit(title_text, title_rect)

        pygame.draw.rect(SCREEN, BLUE, (200, 200, 200, 50))
        text = FONT.render("Human vs Human", True, WHITE)
        text_rect = text.get_rect(center=(300, 225))
        SCREEN.blit(text, text_rect)

        pygame.draw.rect(SCREEN, RED, (200, 260, 200, 50))
        text = FONT.render("Human vs AI", True, WHITE)
        text_rect = text.get_rect(center=(300, 285))
        SCREEN.blit(text, text_rect)

        pygame.draw.rect(SCREEN, GREEN, (200, 320, 200, 50))
        text = FONT.render("AI vs AI", True, WHITE)
        text_rect = text.get_rect(center=(300, 345))
        SCREEN.blit(text, text_rect)

    def draw_game_playing(self):
        SCREEN.fill(BLACK)
        for r in range(GRID_HEIGHT):
            for c in range(GRID_WIDTH):
                cell_renderer = FrontendCellRenderer(self.board.grid[r][c])
                cell_renderer.draw(SCREEN)

        pygame.draw.rect(SCREEN, GRAY, (0, 0, WINDOW_WIDTH, TOOLBAR_HEIGHT))
        player_name = PLAYER_NAMES.get(self.board.current_player, "Unknown")
        player_color_surf = FONT.render(f"Current Player: {player_name}", True, ORB_COLORS.get(self.board.current_player, BLACK))
        player_rect = player_color_surf.get_rect(center=(WINDOW_WIDTH // 2, TOOLBAR_HEIGHT // 2))
        SCREEN.blit(player_color_surf, player_rect)

    def draw_game_over(self):
        SCREEN.fill(BLACK)
        winner_name = PLAYER_NAMES.get(self.board.winner, "DRAW")
        winner_text = FONT.render(f"{winner_name}", True, WHITE)
        winner_rect = winner_text.get_rect(center=(WINDOW_WIDTH // 2, WINDOW_HEIGHT // 2 - 50))
        SCREEN.blit(winner_text, winner_rect)

        # Display match duration
        duration_text = SMALL_FONT.render(f"Match Duration: {self.match_duration:.2f} seconds", True, WHITE)
        duration_rect = duration_text.get_rect(center=(WINDOW_WIDTH // 2, WINDOW_HEIGHT // 2 - 10))
        SCREEN.blit(duration_text, duration_rect)


        pygame.draw.rect(SCREEN, BLUE, (WINDOW_WIDTH // 2 - 75, WINDOW_HEIGHT // 2 + 50, 150, 50))
        text = FONT.render("Play Again", True, WHITE)
        text_rect = text.get_rect(center=(WINDOW_WIDTH // 2, WINDOW_HEIGHT // 2 + 75))
        SCREEN.blit(text, text_rect)

        pygame.draw.rect(SCREEN, RED, (WINDOW_WIDTH // 2 - 75, WINDOW_HEIGHT // 2 + 110, 150, 50))
        quit_text = FONT.render("Quit", True, WHITE)
        quit_text_rect = quit_text.get_rect(center=(WINDOW_WIDTH // 2, WINDOW_HEIGHT // 2 + 135))
        SCREEN.blit(quit_text, quit_text_rect)

    def run(self):
        running = True
        while running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False

                if self.state == MENU:
                    self.handle_menu_input(event)
                elif self.state == GAME_PLAYING:
                    self.handle_game_playing_input(event)
                elif self.state == GAME_OVER:
                    if self.handle_game_over_input(event):
                        running = False

            self.update()

            if self.state == MENU:
                self.draw_menu()
            elif self.state == GAME_PLAYING:
                self.draw_game_playing()
            elif self.state == GAME_OVER:
                self.draw_game_over()

            pygame.display.flip()
            CLOCK.tick(30)

        pygame.quit()
        sys.exit()

if __name__ == "__main__":
    game = GameFrontend()
    game.run()
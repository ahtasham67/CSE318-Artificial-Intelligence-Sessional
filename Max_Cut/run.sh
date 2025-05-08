#!/bin/bash

# Set paths
OUTPUT_FILE="results.csv"
INPUT_DIR="set1"
SOURCE_FILE="main.cpp"
EXECUTABLE="run_stats"

# Step 1: Compile
echo "🛠️  Compiling $SOURCE_FILE..."
g++ -std=c++17 "$SOURCE_FILE" -o "$EXECUTABLE"
if [[ $? -ne 0 ]]; then
    echo "❌ Compilation failed. Exiting."
    exit 1
fi
echo "✅ Compilation successful."

# Step 2: Write CSV header
echo "📝 Generating CSV header..."
cat <<EOL > "$OUTPUT_FILE"
Problem,,,Constructive algorithm,,,Local search,,GRASP,,Known best solution or upper bound
Name,|V| or n,|E| or m,Simple Randomized,Simple Greedy,Semi-greedy-1,Simple local,,GRASP-1,,
,,,,,,No. of iterations,Average value,No. of iterations,Best value,
EOL

# Step 3: Process each graph file (sorted naturally)
echo "📂 Processing input files from $INPUT_DIR..."
find "$INPUT_DIR" -type f | sort -V | while read -r filepath; do
    echo "➡️  Processing $(basename "$filepath")..."
    "./$EXECUTABLE" "$filepath" "$OUTPUT_FILE" 10
done

echo "✅ All files processed. Results saved in $OUTPUT_FILE."

#!/bin/bash
# Convert all digit PBM images to PNG format

# Check if ImageMagick is installed
if ! command -v convert &> /dev/null; then
    echo "Error: ImageMagick is not installed. Please install it first."
    echo "On Ubuntu/Debian: sudo apt-get install imagemagick"
    echo "On macOS: brew install imagemagick"
    exit 1
fi

echo "Converting PBM images to PNG..."

for file in digits/N*.pbm; do
    if [ -f "$file" ]; then
        png_file="${file%.pbm}.png"
        echo "Converting: $file -> $png_file"
        convert "$file" "$png_file"
        
        if [ $? -eq 0 ]; then
            echo "✓ Successfully converted: $file"
        else
            echo "✗ Failed to convert: $file"
        fi
    fi
done

echo "Conversion complete!"
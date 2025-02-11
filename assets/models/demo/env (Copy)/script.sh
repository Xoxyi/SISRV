for file in *.exr; do
    if [[ -f "$file" ]]; then
        output="${file%.exr}.jpg"
        magick "$file" -quality 95 "$output"
        echo "Converted: $file -> $output"
    fi
done

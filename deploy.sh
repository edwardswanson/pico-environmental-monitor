#!/bin/bash
set -e

echo "🔨 Building project..."
cd build
cmake .. > /dev/null 2>&1
make -j4

echo "📤 Flashing to Pico..."
picotool load -f -x lcd_demo.uf2

echo "✅ Deployment complete!"
# IonConnect Web Asset Builder

This tool processes web assets (HTML, CSS, JavaScript) and converts them into C++ header files that can be embedded in the firmware as PROGMEM constants.

## Features

- **Minification**: Reduces HTML and JavaScript file sizes
- **Compression**: Optional gzip compression
- **C++ Conversion**: Generates PROGMEM byte arrays
- **Automatic**: Processes all configured assets

## Installation

```bash
cd tools/web_builder
pip install -r requirements.txt
```

## Usage

```bash
python build.py
```

This will:
1. Read source files from `src/web/assets_src/`
2. Minify HTML and JavaScript
3. Generate C++ headers in `src/web/assets/`

## Configuration

Edit `build.py` to add or modify assets:

```python
FILES = {
    'index.html': {
        'output': 'embedded_html.h',
        'var_name': 'EMBEDDED_HTML',
        'minify': 'html'
    },
    'script.js': {
        'output': 'embedded_html.h',
        'var_name': 'EMBEDDED_JS',
        'minify': 'js'
    }
}
```

## Output Format

Generated header files contain:

```cpp
const char EMBEDDED_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>...
)rawliteral";
```

## Development Workflow

1. **Edit** source files in `src/web/assets_src/`
2. **Build** with `python build.py`
3. **Compile** firmware with new assets
4. **Test** on device

## Advanced Features

### Gzip Compression

Enable compression for smaller firmware size:

```python
# In build.py
def build_asset_compressed(filename, config):
    content_bytes = content.encode('utf-8')
    compressed = gzip.compress(content_bytes, compresslevel=9)
    return bytes_to_c_array(compressed, config['var_name'])
```

### Asset Variants

Create multiple asset versions:

```python
FILES = {
    'index.html': {...},
    'index_minimal.html': {...},  # Lightweight version
}
```

## Troubleshooting

**Import Error:**
```
pip install htmlmin jsmin
```

**Path Issues:**
- Verify `SOURCE_DIR` and `OUTPUT_DIR` paths
- Run from `tools/web_builder/` directory

**Large Assets:**
- Check PROGMEM limits (ESP8266: ~1MB, ESP32: ~4MB)
- Use LittleFS for very large files
- Enable gzip compression

## Integration

### Arduino IDE

1. Run build script before compiling
2. Assets auto-included via `#include "web/assets/embedded_html.h"`

### PlatformIO

Add to `platformio.ini`:

```ini
extra_scripts = pre:tools/web_builder/build.py
```

### Makefile

```makefile
build-assets:
    python tools/web_builder/build.py

firmware: build-assets
    pio run
```


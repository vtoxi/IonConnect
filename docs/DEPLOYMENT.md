# IonConnect Deployment Guide

This guide covers building, testing, and publishing IonConnect to Arduino Library Manager and PlatformIO Registry.

---

## 🛠️ Local Build & Test

### Prerequisites

```bash
# Install PlatformIO
pip install platformio

# Install web asset builder dependencies
cd tools/web_builder
pip install -r requirements.txt
cd ../..
```

### Build All Examples

```bash
# Make scripts executable (first time only)
chmod +x scripts/*.sh

# Build all examples for ESP32 and ESP8266
./scripts/build.sh
```

This will:
- Build web assets
- Compile all 9 examples
- Report build status
- Show memory usage

### Run Tests

```bash
./scripts/test.sh
```

This validates:
- Library structure
- Required files
- Metadata files (library.properties, library.json)
- Example sketches
- Documentation
- Code quality

---

## 📦 Publishing to PlatformIO

### One-Time Setup

1. **Create PlatformIO Account**
   - Go to: https://platformio.org/
   - Sign up / Sign in

2. **Generate Access Token**
   - Go to: https://platformio.org/account/token
   - Create new token
   - Copy token value

3. **Set Environment Variable**
   ```bash
   export PLATFORMIO_AUTH_TOKEN=your_token_here
   # Or add to ~/.bashrc or ~/.zshrc for persistence
   ```

### Manual Publishing

```bash
# Ensure library.json is up to date
cat library.json

# Publish
pio pkg publish --type=library --owner=yourusername

# Or use the publish script
./scripts/publish.sh
```

### Automated Publishing (GitHub Actions)

When you push a version tag, GitHub Actions will automatically publish:

```bash
# Update version in library.properties and library.json
# Commit changes
git add library.properties library.json CHANGELOG.md
git commit -m "Bump version to 1.0.0"

# Create and push tag
git tag -a v1.0.0 -m "Release version 1.0.0"
git push origin main
git push origin v1.0.0
```

The `.github/workflows/release.yml` workflow will:
1. Create GitHub release
2. Publish to PlatformIO (if token is set)
3. Create release ZIP for Arduino

---

## 📚 Publishing to Arduino Library Manager

### Prerequisites

1. **Library hosted on GitHub**
   - Repository must be public
   - Must have proper library.properties
   - Must follow Arduino library specification

2. **Valid library.properties**
   ```properties
   name=IonConnect
   version=1.0.0
   author=Your Name <email@example.com>
   maintainer=Your Name <email@example.com>
   sentence=Modern WiFi provisioning for ESP32/ESP8266
   paragraph=...
   category=Communication
   url=https://github.com/yourusername/IonConnect
   architectures=esp32,esp8266
   depends=ESPAsyncWebServer,ArduinoJson
   ```

### Submission Methods

#### Method 1: Via Pull Request (Recommended)

1. **Fork the Registry**
   - Go to: https://github.com/arduino/library-registry
   - Click "Fork"

2. **Add Your Library**
   - Edit `repositories.txt`
   - Add line: `https://github.com/yourusername/IonConnect`
   - Commit change

3. **Create Pull Request**
   - Submit PR to main repository
   - Wait for review (usually 1-3 days)

#### Method 2: Via Issue

1. **Create Issue**
   - Go to: https://github.com/arduino/library-registry/issues/new
   - Title: "Add IonConnect library"
   - Body:
     ```
     Repository: https://github.com/yourusername/IonConnect
     Description: Modern WiFi provisioning for ESP32/ESP8266
     ```

2. **Wait for Review**
   - Maintainers will review and add

### Verification

Once approved, your library will appear in:
- Arduino IDE Library Manager
- Arduino CLI: `arduino-cli lib search IonConnect`
- Arduino website: https://www.arduino.cc/reference/en/libraries/

---

## 🚀 Release Process

### Pre-Release Checklist

- [ ] All tests pass (`./scripts/test.sh`)
- [ ] All examples build (`./scripts/build.sh`)
- [ ] Update CHANGELOG.md with new version
- [ ] Update version in library.properties
- [ ] Update version in library.json
- [ ] Update version in README.md badges
- [ ] Review and update documentation
- [ ] Test on real hardware (ESP32 & ESP8266)
- [ ] Review security (no hardcoded tokens/keys)

### Release Steps

1. **Update Version Numbers**
   ```bash
   # Edit library.properties
   version=1.0.0
   
   # Edit library.json
   "version": "1.0.0"
   
   # Update CHANGELOG.md
   ## [1.0.0] - 2025-10-31
   ```

2. **Run Full Test Suite**
   ```bash
   ./scripts/test.sh
   ./scripts/build.sh
   ```

3. **Commit & Tag**
   ```bash
   git add -A
   git commit -m "Release v1.0.0"
   git tag -a v1.0.0 -m "Release version 1.0.0"
   git push origin main
   git push origin v1.0.0
   ```

4. **Create GitHub Release**
   - Go to: https://github.com/yourusername/IonConnect/releases/new
   - Tag: v1.0.0
   - Title: IonConnect v1.0.0
   - Description: Copy from CHANGELOG.md
   - Attach: IonConnect-1.0.0.zip
   - Publish release

5. **Publish to PlatformIO**
   ```bash
   pio pkg publish --type=library
   ```

6. **Submit to Arduino**
   - Follow Arduino Library Manager submission process above

### Automated Release

For automated releases, use the publish script:

```bash
./scripts/publish.sh
```

This script will:
1. Run all tests
2. Build all examples
3. Create release package
4. Create git tag
5. Push to GitHub
6. Publish to PlatformIO (if configured)
7. Show Arduino submission instructions

---

## 🔄 Continuous Integration

### GitHub Actions Workflows

#### CI Workflow (`.github/workflows/ci.yml`)

Runs on every push and pull request:
- Builds all examples for ESP32 and ESP8266
- Runs code quality checks
- Validates library structure
- Checks memory usage

#### Release Workflow (`.github/workflows/release.yml`)

Runs when you push a version tag (e.g., `v1.0.0`):
- Creates GitHub release
- Publishes to PlatformIO
- Generates release ZIP
- Builds documentation

### Setup GitHub Secrets

For automated publishing, add secrets to your GitHub repository:

1. Go to: Settings → Secrets and variables → Actions

2. Add secrets:
   - `PLATFORMIO_AUTH_TOKEN` - Your PlatformIO token

3. Workflows will use these automatically

---

## 📊 Version Management

### Semantic Versioning

IonConnect follows [Semantic Versioning](https://semver.org/):

- **MAJOR.MINOR.PATCH** (e.g., 1.0.0)
- **MAJOR**: Breaking changes
- **MINOR**: New features (backward compatible)
- **PATCH**: Bug fixes

### Version Update Locations

When releasing a new version, update:

1. `library.properties` → `version=1.0.0`
2. `library.json` → `"version": "1.0.0"`
3. `CHANGELOG.md` → Add new section
4. `src/core/IonConfig.h` → `ION_VERSION`
5. README badges (if using shields.io)

---

## 🧪 Testing Before Release

### Local Testing

```bash
# Run automated tests
./scripts/test.sh

# Build all examples
./scripts/build.sh

# Test specific example
cd examples/BasicSetup
pio run -e esp32 -t upload -t monitor
```

### Hardware Testing

Follow the comprehensive guide in `docs/HARDWARE_TESTING.md`:

1. **Basic Functionality**
   - First boot & portal
   - WiFi configuration
   - Configuration persistence

2. **Advanced Features**
   - Multi-network management
   - Security features
   - BLE provisioning (ESP32)
   - OTA updates

3. **Stability Testing**
   - Memory monitoring
   - 24-hour uptime test
   - Edge cases

---

## 📈 Post-Release

### Monitoring

- **GitHub**: Watch issues and pull requests
- **PlatformIO**: Monitor download stats
- **Arduino**: Check library listing

### Promotion

- [ ] Announce on Arduino forums
- [ ] Post on Reddit (r/arduino, r/esp32, r/esp8266)
- [ ] Share on Twitter/X
- [ ] Update project website/blog
- [ ] Create demo video (YouTube)

### Support

- Respond to GitHub issues
- Update documentation based on feedback
- Plan next version features

---

## 🔧 Troubleshooting

### Build Failures

**Problem**: Example won't compile

**Solutions**:
- Check PlatformIO version: `pio upgrade`
- Clear build cache: `pio run -t clean`
- Update dependencies: `pio pkg update`

### Publishing Issues

**Problem**: PlatformIO authentication failed

**Solutions**:
- Regenerate token at https://platformio.org/account/token
- Check token is set: `echo $PLATFORMIO_AUTH_TOKEN`
- Use manual publishing: `pio pkg publish`

**Problem**: Arduino Library Manager rejected

**Solutions**:
- Validate library.properties format
- Ensure GitHub repository is public
- Check all required fields are present
- Review Arduino library specification

### GitHub Actions Failures

**Problem**: CI workflow fails

**Solutions**:
- Check workflow logs in Actions tab
- Run tests locally: `./scripts/test.sh`
- Verify all required files exist
- Check for syntax errors in workflows

---

## 📞 Support

**Documentation:**
- README.md
- HARDWARE_TESTING.md
- This deployment guide

**Community:**
- GitHub Issues: Report bugs
- GitHub Discussions: Ask questions
- Pull Requests: Contribute improvements

---

## ✅ Deployment Checklist

Use this checklist for each release:

### Pre-Release
- [ ] All tests pass
- [ ] All examples build
- [ ] Hardware testing completed
- [ ] Documentation updated
- [ ] CHANGELOG.md updated
- [ ] Version numbers updated

### Release
- [ ] Git tag created
- [ ] GitHub release created
- [ ] Published to PlatformIO
- [ ] Submitted to Arduino
- [ ] Release notes published

### Post-Release
- [ ] Announced on forums/social
- [ ] Monitoring for issues
- [ ] Updated project website
- [ ] Planning next version

---

**Happy Deploying! 🚀**


# 📤 IonConnect Publishing Instructions

Follow these step-by-step instructions to publish IonConnect to Arduino and PlatformIO registries.

---

## ✅ Prerequisites Checklist

Before publishing, ensure:

- [ ] All tests pass (`./scripts/test.sh` shows 43/43 ✓)
- [ ] You have a GitHub account
- [ ] Repository is pushed to GitHub
- [ ] You have PlatformIO account (for PlatformIO publishing)
- [ ] You've reviewed CHANGELOG.md
- [ ] Version numbers are correct in library.properties and library.json

---

## 🚀 Method 1: Automated Publishing (Recommended)

### Step 1: Prepare Your Repository

```bash
# Ensure you're on main branch
git checkout main

# Verify all files are committed
git status

# If there are changes, commit them
git add -A
git commit -m "Prepare for v1.0.0 release"
```

### Step 2: Run Automated Publish Script

```bash
# Make sure you're in the project root
cd /path/to/IonConnect

# Run the publish script
./scripts/publish.sh
```

The script will:
1. ✅ Run all 43 tests
2. ✅ Build all examples
3. ✅ Create release package (IonConnect-1.0.0.zip)
4. ✅ Create git tag (v1.0.0)
5. ✅ Push to GitHub
6. ✅ Publish to PlatformIO (if token configured)
7. ℹ️ Show Arduino submission instructions

### Step 3: Follow the Prompts

The script will ask you to confirm each step. Answer:
- Run tests? → **y**
- Build examples? → **y**
- Commit changes? → **y** (if needed)
- Push to GitHub? → **y**
- Publish to PlatformIO? → **y** (if you have token)

### Step 4: PlatformIO Token Setup (First Time Only)

If publishing to PlatformIO for the first time:

1. Go to: https://platformio.org/account/token
2. Click "Generate New Token"
3. Copy the token
4. When prompted by script, paste token

Or set permanently:
```bash
# Add to ~/.bashrc or ~/.zshrc
export PLATFORMIO_AUTH_TOKEN="your_token_here"
```

---

## 🛠️ Method 2: Manual Publishing

### Option A: Manual PlatformIO Publishing

```bash
# Set your token (if not already set)
export PLATFORMIO_AUTH_TOKEN="your_token_here"

# Publish
pio pkg publish --type=library

# Or with owner specified
pio pkg publish --type=library --owner=yourusername
```

**Verify Publication:**
- Go to: https://registry.platformio.org/libraries/yourusername/IonConnect
- Library should appear within minutes

### Option B: Manual Arduino Publishing

#### Via Pull Request (Recommended):

1. **Fork the Arduino Library Registry**
   ```
   https://github.com/arduino/library-registry
   ```

2. **Edit `repositories.txt`**
   - Add this line:
   ```
   https://github.com/yourusername/IonConnect
   ```

3. **Create Pull Request**
   - Title: "Add IonConnect library"
   - Description:
   ```
   Adding IonConnect - Modern WiFi provisioning library for ESP32/ESP8266
   
   Repository: https://github.com/yourusername/IonConnect
   Category: Communication
   Architectures: ESP32, ESP8266
   ```

4. **Wait for Review**
   - Typically takes 1-3 business days
   - Maintainers will review and merge

#### Via Issue:

1. **Create New Issue**
   - Go to: https://github.com/arduino/library-registry/issues/new

2. **Fill in Details**
   - Title: "Add IonConnect library"
   - Body:
   ```
   **Library Name**: IonConnect
   **Repository URL**: https://github.com/yourusername/IonConnect
   **Description**: Modern, async WiFi provisioning library for ESP32/ESP8266
   **Category**: Communication
   **License**: MIT
   ```

---

## 📋 GitHub Release (Manual Method)

If not using automated scripts:

### Step 1: Create Release Package

```bash
# Create release directory
mkdir -p release/IonConnect

# Copy files
cp -r src release/IonConnect/
cp -r examples release/IonConnect/
cp library.properties release/IonConnect/
cp library.json release/IonConnect/
cp keywords.txt release/IonConnect/
cp README.md release/IonConnect/
cp LICENSE release/IonConnect/
cp CHANGELOG.md release/IonConnect/

# Create ZIP
cd release
zip -r ../IonConnect-1.0.0.zip IonConnect/
cd ..
```

### Step 2: Create Git Tag

```bash
# Create annotated tag
git tag -a v1.0.0 -m "Release version 1.0.0"

# Push tag to GitHub
git push origin v1.0.0
```

### Step 3: Create GitHub Release

1. Go to: `https://github.com/yourusername/IonConnect/releases/new`

2. Fill in details:
   - **Tag**: Select `v1.0.0`
   - **Title**: `IonConnect v1.0.0`
   - **Description**: Copy from CHANGELOG.md

3. **Attach Files**:
   - Upload `IonConnect-1.0.0.zip`

4. **Publish Release**

---

## 🔄 Using GitHub Actions (Automated CI/CD)

### Setup (One-Time)

1. **Add Secrets to Repository**
   - Go to: Settings → Secrets and variables → Actions
   - Click "New repository secret"
   - Add:
     - Name: `PLATFORMIO_AUTH_TOKEN`
     - Value: Your PlatformIO token

2. **Workflows Are Already Configured**
   - `.github/workflows/ci.yml` - Runs on every push
   - `.github/workflows/release.yml` - Runs on version tags

### Trigger Automated Release

```bash
# Simply push a version tag
git tag -a v1.0.0 -m "Release version 1.0.0"
git push origin v1.0.0
```

GitHub Actions will automatically:
- ✅ Run all tests
- ✅ Build all examples
- ✅ Create GitHub release
- ✅ Publish to PlatformIO
- ✅ Generate documentation

**Monitor Progress:**
- Go to: Repository → Actions tab
- Watch the "Release & Publish" workflow

---

## ✅ Post-Publishing Verification

### Verify PlatformIO

1. **Check Registry**
   ```
   https://registry.platformio.org/libraries/yourusername/IonConnect
   ```

2. **Test Installation**
   ```bash
   # Create test project
   pio project init --board esp32dev
   
   # Install library
   pio pkg install --library "yourusername/IonConnect"
   
   # Should download and install successfully
   ```

### Verify Arduino (After Approval)

1. **Check Library Manager**
   - Open Arduino IDE
   - Tools → Manage Libraries...
   - Search "IonConnect"
   - Should appear in results

2. **Test Installation**
   - Click "Install"
   - File → Examples → IonConnect
   - Examples should be available

### Verify GitHub Release

1. **Check Releases Page**
   ```
   https://github.com/yourusername/IonConnect/releases
   ```

2. **Verify**:
   - Release exists with correct version
   - ZIP file is attached
   - Description is correct
   - Tag is correct

---

## 🐛 Troubleshooting

### PlatformIO Publishing Issues

**Problem**: "Authentication failed"
```bash
# Solution: Regenerate token
# 1. Go to: https://platformio.org/account/token
# 2. Delete old token
# 3. Create new token
# 4. Set environment variable:
export PLATFORMIO_AUTH_TOKEN="new_token_here"
```

**Problem**: "Library already exists"
```bash
# Solution: Bump version number
# 1. Edit library.properties: version=1.0.1
# 2. Edit library.json: "version": "1.0.1"
# 3. Update CHANGELOG.md
# 4. Create new tag: git tag v1.0.1
```

### Arduino Submission Issues

**Problem**: PR rejected due to formatting
```bash
# Solution: Verify library.properties format
cat library.properties

# Must have:
name=IonConnect
version=1.0.0
author=Name <email>
maintainer=Name <email>
sentence=Short description
paragraph=Long description
category=Communication
url=https://github.com/yourusername/IonConnect
architectures=esp32,esp8266
depends=ESPAsyncWebServer,ArduinoJson
```

**Problem**: "Repository not found"
```bash
# Solution: Ensure repository is public
# Go to: Repository Settings → General
# Make sure "Public" is selected
```

### GitHub Actions Issues

**Problem**: Workflow fails
```bash
# Solution: Check workflow logs
# 1. Go to: Actions tab
# 2. Click failed workflow
# 3. View logs
# 4. Fix issues in code
# 5. Push fix
```

---

## 📊 Publishing Checklist

Use this checklist when publishing:

### Pre-Publishing
- [ ] All tests pass (43/43)
- [ ] Version updated in library.properties
- [ ] Version updated in library.json
- [ ] CHANGELOG.md updated
- [ ] All changes committed
- [ ] Repository pushed to GitHub

### PlatformIO
- [ ] Token configured
- [ ] Published successfully
- [ ] Verified on registry
- [ ] Test installation works

### Arduino
- [ ] Submitted to library registry
- [ ] Waiting for approval
- [ ] Approved and listed
- [ ] Test installation works

### GitHub
- [ ] Tag created
- [ ] Release created
- [ ] ZIP attached
- [ ] Description complete

### Documentation
- [ ] README updated
- [ ] Examples documented
- [ ] API docs current
- [ ] Changelog complete

### Community
- [ ] Announced on forums
- [ ] Shared on social media
- [ ] Monitoring for issues
- [ ] Ready to provide support

---

## 🎉 Congratulations!

Once published, your library will be available to thousands of developers worldwide!

### What's Next?

1. **Monitor** - Watch for issues and feedback
2. **Support** - Respond to questions
3. **Update** - Plan future features
4. **Promote** - Share with community
5. **Maintain** - Keep documentation current

---

## 📞 Support Resources

**Documentation:**
- README.md - User guide
- DEPLOYMENT.md - Technical deployment
- HARDWARE_TESTING.md - Testing procedures

**Community:**
- GitHub Issues - Bug reports
- GitHub Discussions - Questions
- Pull Requests - Contributions

**Publishing Help:**
- PlatformIO Docs: https://docs.platformio.org/en/latest/librarymanager/
- Arduino Library Specification: https://arduino.github.io/arduino-cli/library-specification/

---

**Happy Publishing! 🚀**



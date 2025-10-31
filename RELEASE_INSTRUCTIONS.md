# Release Instructions for IonConnect-ESP

## Version Numbering

Follow [Semantic Versioning](https://semver.org/):
- **MAJOR** (x.0.0): Breaking changes to API
- **MINOR** (1.x.0): New features, backward compatible
- **PATCH** (1.0.x): Bug fixes, backward compatible

## Release Checklist

### 1. Update Version Numbers

Update version in both files:
- [ ] `library.json` → `"version": "1.x.0"`
- [ ] `library.properties` → `version=1.x.0`

### 2. Update CHANGELOG.md

Add new version entry:
```markdown
## [1.x.0] - YYYY-MM-DD

### Added
- New feature descriptions

### Changed
- Modified behavior descriptions

### Fixed
- Bug fix descriptions
```

### 3. Commit Changes

```bash
git add library.json library.properties CHANGELOG.md
git commit -m "Bump version to 1.x.0"
git push origin main
```

### 4. Create and Push Tag

```bash
# Create annotated tag
git tag -a v1.x.0 -m "Release v1.x.0: Brief description"

# Push tag to trigger GitHub Actions
git push origin v1.x.0
```

### 5. GitHub Actions Will Automatically:
- ✅ Validate library structure
- ✅ Publish to PlatformIO registry
- ✅ Create GitHub release with notes
- ✅ Attach package artifacts

### 6. Verify Publication

1. **Check GitHub Actions**: https://github.com/vtoxi/IonConnect/actions
2. **Check PlatformIO**: https://registry.platformio.org/libraries/vtoxi/IonConnect-ESP
3. **Verify GitHub Release**: https://github.com/vtoxi/IonConnect/releases

## First-Time Setup

### Configure GitHub Secrets

Add PlatformIO authentication token:

1. Get your PlatformIO token:
   ```bash
   pio account token
   ```

2. Add to GitHub:
   - Go to: https://github.com/vtoxi/IonConnect/settings/secrets/actions
   - Click: "New repository secret"
   - Name: `PLATFORMIO_AUTH_TOKEN`
   - Value: [paste your token]
   - Click: "Add secret"

### Register PlatformIO Organization (Optional)

If publishing under organization:
```bash
pio org create vtoxi
```

## Example Release Process

### Minor Version (New Feature)

```bash
# 1. Update versions to 1.2.0
# 2. Update CHANGELOG.md
# 3. Commit
git add .
git commit -m "Bump version to 1.2.0"
git push origin main

# 4. Tag and push
git tag -a v1.2.0 -m "Release v1.2.0: Add new feature XYZ"
git push origin v1.2.0

# 5. Wait for GitHub Actions to complete
# 6. Verify on PlatformIO registry
```

### Patch Version (Bug Fix)

```bash
# 1. Update versions to 1.1.1
# 2. Update CHANGELOG.md
# 3. Commit and tag
git add .
git commit -m "Bump version to 1.1.1 - Fix crash on ESP-01"
git push origin main
git tag -a v1.1.1 -m "Release v1.1.1: Fix ESP-01 initialization crash"
git push origin v1.1.1
```

## Rollback a Release

If something goes wrong:

```bash
# Delete tag locally
git tag -d v1.x.0

# Delete tag on remote
git push origin :refs/tags/v1.x.0

# Delete release on GitHub (manually via web UI)
# PlatformIO packages cannot be deleted, only yanked
```

## Version History

- **v1.1.0** (2025-10-31) - Minimal mode for ESP-01 and low-memory devices
- **v1.0.2** (2025-10-31) - Library renamed to IonConnect-ESP
- **v1.0.1** (2025-10-30) - Initial release

## Support

- Issues: https://github.com/vtoxi/IonConnect/issues
- Discussions: https://github.com/vtoxi/IonConnect/discussions

---

**Note**: Always test thoroughly before releasing!


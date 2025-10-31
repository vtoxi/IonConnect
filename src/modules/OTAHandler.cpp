#include "OTAHandler.h"

#if ION_ENABLE_OTA

#include "../utils/Logger.h"

namespace IonConnect {

OTAHandler::OTAHandler(SecurityManager* security) 
    : security(security), updateProgress(0), updateTotal(0), updateInProgress(false) {
}

OTAHandler::~OTAHandler() {
}

bool OTAHandler::begin(AsyncWebServer* server) {
    if (!server) {
        ION_LOG_E("Invalid server");
        return false;
    }
    
    setupRoutes(server);
    ION_LOG("OTA handler initialized");
    
    return true;
}

void OTAHandler::setUpdateURL(const String& url) {
    updateURL = url;
}

void OTAHandler::onStart(std::function<void()> cb) {
    startCallback = cb;
}

void OTAHandler::onProgress(std::function<void(size_t, size_t)> cb) {
    progressCallback = cb;
}

void OTAHandler::onEnd(std::function<void()> cb) {
    endCallback = cb;
}

void OTAHandler::onError(std::function<void(int)> cb) {
    errorCallback = cb;
}

void OTAHandler::setupRoutes(AsyncWebServer* server) {
    // Update page
    server->on("/update", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleUpdatePage(request);
    });
    
    // Update upload
    server->on("/update", HTTP_POST, 
        [this](AsyncWebServerRequest* request) {
            bool success = !Update.hasError();
            
            AsyncWebServerResponse* response = request->beginResponse(200, "text/html",
                success ? 
                "<html><body><h1>Update Success!</h1><p>Device will reboot...</p></body></html>" :
                "<html><body><h1>Update Failed!</h1><p>Error occurred</p></body></html>"
            );
            
            response->addHeader("Connection", "close");
            request->send(response);
            
            if (success) {
                delay(1000);
                ESP.restart();
            }
        },
        [this](AsyncWebServerRequest* request, String filename, size_t index, 
               uint8_t* data, size_t len, bool final) {
            handleUpdateUpload(request, filename, index, data, len, final);
        }
    );
}

void OTAHandler::handleUpdatePage(AsyncWebServerRequest* request) {
    // Check authentication
    if (security && !security->authorizeRequest(request)) {
        request->send(401, "text/html", "<html><body><h1>Unauthorized</h1></body></html>");
        return;
    }
    
    const char* html = R"html(
<!DOCTYPE html>
<html>
<head>
    <title>OTA Update</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial; max-width: 500px; margin: 50px auto; padding: 20px; }
        .progress { width: 100%; height: 30px; background: #ddd; border-radius: 5px; overflow: hidden; display: none; }
        .progress-bar { height: 100%; background: #4CAF50; width: 0%; transition: width 0.3s; text-align: center; color: white; line-height: 30px; }
        input[type="file"] { margin: 20px 0; }
        button { background: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; }
        button:hover { background: #45a049; }
    </style>
</head>
<body>
    <h1>🔧 Firmware Update</h1>
    <p>Select a firmware file (.bin) to upload:</p>
    <form method="POST" action="/update" enctype="multipart/form-data" id="upload-form">
        <input type="file" name="update" accept=".bin" id="file" required>
        <button type="submit">Upload</button>
    </form>
    <div class="progress" id="progress">
        <div class="progress-bar" id="progress-bar">0%</div>
    </div>
    <p id="status"></p>
    <script>
        const form = document.getElementById('upload-form');
        const progress = document.getElementById('progress');
        const progressBar = document.getElementById('progress-bar');
        const status = document.getElementById('status');
        
        form.addEventListener('submit', (e) => {
            e.preventDefault();
            const formData = new FormData(form);
            const xhr = new XMLHttpRequest();
            
            xhr.upload.addEventListener('progress', (e) => {
                if (e.lengthComputable) {
                    const percent = (e.loaded / e.total) * 100;
                    progress.style.display = 'block';
                    progressBar.style.width = percent + '%';
                    progressBar.textContent = Math.round(percent) + '%';
                }
            });
            
            xhr.addEventListener('load', () => {
                if (xhr.status === 200) {
                    status.textContent = 'Update successful! Device rebooting...';
                    status.style.color = 'green';
                } else {
                    status.textContent = 'Update failed!';
                    status.style.color = 'red';
                }
            });
            
            xhr.open('POST', '/update');
            xhr.send(formData);
        });
    </script>
</body>
</html>
)html";
    
    request->send(200, "text/html", html);
}

void OTAHandler::handleUpdateUpload(AsyncWebServerRequest* request, String filename,
                                   size_t index, uint8_t* data, size_t len, bool final) {
    if (index == 0) {
        // Start update
        ION_LOG("OTA Update Start: %s", filename.c_str());
        
        updateInProgress = true;
        updateProgress = 0;
        
        #if ION_PLATFORM_ESP32
            size_t updateSize = UPDATE_SIZE_UNKNOWN;
            if (!Update.begin(updateSize)) {
        #elif ION_PLATFORM_ESP8266
            uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
            if (!Update.begin(maxSketchSpace)) {
        #endif
            
            Update.printError(Serial);
            if (errorCallback) errorCallback(Update.getError());
            updateInProgress = false;
            return;
        }
        
        if (startCallback) startCallback();
    }
    
    if (updateInProgress) {
        // Write chunk
        if (Update.write(data, len) != len) {
            Update.printError(Serial);
            if (errorCallback) errorCallback(Update.getError());
            updateInProgress = false;
            return;
        }
        
        updateProgress += len;
        
        if (progressCallback) {
            progressCallback(updateProgress, updateTotal);
        }
        
        // Log progress every 10%
        static size_t lastPercent = 0;
        size_t percent = (updateProgress * 100) / Update.size();
        if (percent != lastPercent && percent % 10 == 0) {
            ION_LOG("OTA Progress: %d%%", percent);
            lastPercent = percent;
        }
    }
    
    if (final) {
        // Finish update
        if (Update.end(true)) {
            ION_LOG("OTA Update Success: %u bytes", updateProgress);
            if (endCallback) endCallback();
        } else {
            Update.printError(Serial);
            if (errorCallback) errorCallback(Update.getError());
        }
        
        updateInProgress = false;
    }
}

bool OTAHandler::validateFirmware(uint8_t* data, size_t len) {
    // Basic validation - check ESP binary magic byte
    if (len < 4) return false;
    
    #if ION_PLATFORM_ESP32
        // ESP32 binary starts with 0xE9
        if (data[0] != 0xE9) return false;
    #elif ION_PLATFORM_ESP8266
        // ESP8266 binary starts with 0xE9
        if (data[0] != 0xE9) return false;
    #endif
    
    return true;
}

} // namespace IonConnect

#endif // ION_ENABLE_OTA


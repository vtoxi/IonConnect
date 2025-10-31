// IonConnect Portal JavaScript
let eventSource = null;

// Initialize on page load
document.addEventListener('DOMContentLoaded', () => {
    loadConfigSchema();
    setupEventListeners();
    connectSSE();
    loadDiagnostics();
});

// Setup event listeners
function setupEventListeners() {
    document.getElementById('scan-btn').addEventListener('click', scanNetworks);
    document.getElementById('save-btn').addEventListener('click', saveConfig);
    document.getElementById('clear-btn').addEventListener('click', clearConfig);
    document.getElementById('export-btn').addEventListener('click', exportConfig);
    document.getElementById('import-btn').addEventListener('click', () => document.getElementById('import-file').click());
    document.getElementById('import-file').addEventListener('change', importConfig);
    document.getElementById('diag-toggle').addEventListener('click', toggleDiagnostics);
}

// Load configuration schema
async function loadConfigSchema() {
    try {
        const response = await fetch('/api/schema');
        const schema = await response.json();
        
        const form = document.getElementById('config-form');
        form.innerHTML = '';
        
        schema.fields.forEach(field => {
            const fieldHTML = generateFieldHTML(field);
            form.insertAdjacentHTML('beforeend', fieldHTML);
        });
        
        // Load existing config
        loadExistingConfig();
        
        // Setup conditional visibility
        setupConditionalVisibility(schema.fields);
        
    } catch (error) {
        console.error('Failed to load schema:', error);
        showToast('Failed to load configuration schema', 'error');
    }
}

// Generate HTML for a form field
function generateFieldHTML(field) {
    const inputClasses = 'w-full p-2 bg-slate-800 border border-slate-700 rounded-lg focus:ring-2 focus:ring-blue-500 focus:outline-none';
    const labelClasses = 'block mb-1 text-sm font-medium';
    const required = field.required ? '<span class="text-red-400">*</span>' : '';
    
    let html = `<div class="field-group" data-field-id="${field.id}">`;
    html += `<label class="${labelClasses}">${field.label} ${required}</label>`;
    
    switch (field.type) {
        case 'text':
        case 'password':
            html += `<input type="${field.type}" name="${field.id}" class="${inputClasses}" 
                     placeholder="${field.placeholder || ''}" 
                     ${field.required ? 'required' : ''}
                     ${field.maxLength ? `maxlength="${field.maxLength}"` : ''}>`;
            break;
            
        case 'number':
            html += `<input type="number" name="${field.id}" class="${inputClasses}" 
                     value="${field.default || ''}"
                     ${field.min !== undefined ? `min="${field.min}"` : ''}
                     ${field.max !== undefined ? `max="${field.max}"` : ''}>`;
            break;
            
        case 'select':
            html += `<select name="${field.id}" class="${inputClasses}">`;
            if (field.options) {
                field.options.forEach(opt => {
                    html += `<option value="${opt}">${opt}</option>`;
                });
            }
            html += `</select>`;
            break;
            
        case 'checkbox':
            html += `<div class="flex items-center">
                        <input type="checkbox" name="${field.id}" class="mr-2 w-4 h-4"
                               ${field.default === 'true' ? 'checked' : ''}>
                        <span class="text-sm text-slate-300">${field.label}</span>
                     </div>`;
            break;
            
        case 'textarea':
            html += `<textarea name="${field.id}" class="${inputClasses}" rows="3"
                     placeholder="${field.placeholder || ''}"></textarea>`;
            break;
    }
    
    html += '</div>';
    return html;
}

// Load existing configuration
async function loadExistingConfig() {
    try {
        const response = await fetch('/api/config');
        const config = await response.json();
        
        // Populate form fields
        Object.keys(config).forEach(key => {
            const input = document.querySelector(`[name="${key}"]`);
            if (input) {
                if (input.type === 'checkbox') {
                    input.checked = config[key] === 'true' || config[key] === true;
                } else {
                    input.value = config[key];
                }
            }
        });
    } catch (error) {
        console.error('Failed to load config:', error);
    }
}

// Setup conditional field visibility
function setupConditionalVisibility(fields) {
    fields.forEach(field => {
        if (field.visible_if) {
            const [depField, depValue] = field.visible_if.split('==');
            const depInput = document.querySelector(`[name="${depField}"]`);
            const fieldGroup = document.querySelector(`[data-field-id="${field.id}"]`);
            
            if (depInput && fieldGroup) {
                const checkVisibility = () => {
                    const currentValue = depInput.type === 'checkbox' 
                        ? depInput.checked.toString() 
                        : depInput.value;
                    fieldGroup.classList.toggle('hidden', currentValue !== depValue);
                };
                
                depInput.addEventListener('change', checkVisibility);
                checkVisibility(); // Initial check
            }
        }
    });
}

// Scan for WiFi networks
async function scanNetworks() {
    const btn = document.getElementById('scan-btn');
    const list = document.getElementById('networks-list');
    
    btn.disabled = true;
    btn.innerHTML = '<span class="spinner inline-block"></span> Scanning...';
    list.innerHTML = '<p class="text-slate-400">Scanning for networks...</p>';
    
    try {
        await fetch('/api/scan', { method: 'POST' });
        
        // Wait for scan to complete
        await new Promise(resolve => setTimeout(resolve, 3000));
        
        const response = await fetch('/api/networks');
        const data = await response.json();
        
        displayNetworks(data.networks || []);
        
    } catch (error) {
        console.error('Scan failed:', error);
        list.innerHTML = '<p class="text-red-400">Scan failed</p>';
        showToast('Network scan failed', 'error');
    } finally {
        btn.disabled = false;
        btn.innerHTML = '📡 Scan';
    }
}

// Display scanned networks
function displayNetworks(networks) {
    const list = document.getElementById('networks-list');
    
    if (networks.length === 0) {
        list.innerHTML = '<p class="text-slate-400">No networks found</p>';
        return;
    }
    
    list.innerHTML = networks.map(net => {
        const signalIcon = getSignalIcon(net.rssi);
        const lockIcon = net.encryption !== 0 ? '🔒' : '';
        
        return `
            <div class="network-item border border-slate-700 mb-2" onclick="selectNetwork('${net.ssid}')">
                <div class="flex items-center gap-3">
                    <span class="text-2xl">${signalIcon}</span>
                    <div class="flex-1">
                        <p class="font-semibold">${net.ssid} ${lockIcon}</p>
                        <p class="text-sm text-slate-400">Channel ${net.channel}</p>
                    </div>
                    <span class="text-sm text-slate-400">${net.rssi} dBm</span>
                </div>
            </div>
        `;
    }).join('');
}

// Get signal strength icon
function getSignalIcon(rssi) {
    if (rssi >= -50) return '📶';
    if (rssi >= -60) return '📶';
    if (rssi >= -70) return '📡';
    return '📡';
}

// Select network
function selectNetwork(ssid) {
    const ssidInput = document.querySelector('[name="wifi_ssid"]');
    const passInput = document.querySelector('[name="wifi_pass"]');
    
    if (ssidInput) {
        ssidInput.value = ssid;
        if (passInput) passInput.focus();
    }
}

// Save configuration
async function saveConfig() {
    const form = document.getElementById('config-form');
    const formData = new FormData(form);
    const config = {};
    
    formData.forEach((value, key) => {
        const input = form.querySelector(`[name="${key}"]`);
        if (input.type === 'checkbox') {
            config[key] = input.checked ? 'true' : 'false';
        } else {
            config[key] = value;
        }
    });
    
    const btn = document.getElementById('save-btn');
    btn.disabled = true;
    btn.innerHTML = '<span class="spinner inline-block"></span> Saving...';
    
    try {
        const response = await fetch('/api/config', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(config)
        });
        
        if (response.ok) {
            showToast('Configuration saved! Connecting...', 'success');
            updateStatus('connecting', config.wifi_ssid);
        } else {
            const error = await response.json();
            showToast(error.error || 'Failed to save configuration', 'error');
            btn.disabled = false;
            btn.innerHTML = '💾 Save & Connect';
        }
        
    } catch (error) {
        console.error('Save failed:', error);
        showToast('Failed to save configuration', 'error');
        btn.disabled = false;
        btn.innerHTML = '💾 Save & Connect';
    }
}

// Clear configuration
async function clearConfig() {
    if (!confirm('Clear all saved configuration?')) return;
    
    try {
        const response = await fetch('/api/clear', { method: 'POST' });
        
        if (response.ok) {
            showToast('Configuration cleared', 'success');
            setTimeout(() => location.reload(), 1500);
        } else {
            showToast('Failed to clear configuration', 'error');
        }
        
    } catch (error) {
        console.error('Clear failed:', error);
        showToast('Failed to clear configuration', 'error');
    }
}

// Export configuration
async function exportConfig() {
    try {
        const response = await fetch('/api/export');
        const blob = await response.blob();
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = `ionconnect-config-${Date.now()}.json`;
        a.click();
        URL.revokeObjectURL(url);
        
        showToast('Configuration exported', 'success');
        
    } catch (error) {
        console.error('Export failed:', error);
        showToast('Failed to export configuration', 'error');
    }
}

// Import configuration
async function importConfig(event) {
    const file = event.target.files[0];
    if (!file) return;
    
    try {
        const text = await file.text();
        const response = await fetch('/api/import', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: text
        });
        
        const result = await response.json();
        
        if (result.success) {
            showToast('Configuration restored!', 'success');
            setTimeout(() => location.reload(), 1500);
        } else {
            showToast(result.error || 'Import failed', 'error');
        }
        
    } catch (error) {
        console.error('Import failed:', error);
        showToast('Failed to import configuration', 'error');
    }
    
    event.target.value = '';
}

// Connect to SSE for live updates
function connectSSE() {
    if (eventSource) eventSource.close();
    
    eventSource = new EventSource('/api/events');
    
    eventSource.addEventListener('status', (e) => {
        const data = JSON.parse(e.data);
        updateStatus(data.state, data.ssid, data.ip, data.message);
    });
    
    eventSource.onerror = (e) => {
        console.error('SSE error:', e);
        // Will auto-reconnect
    };
}

// Update status display
function updateStatus(state, ssid, ip, message) {
    const statusContent = document.getElementById('status-content');
    const statusBadge = document.getElementById('status-badge');
    const saveBtn = document.getElementById('save-btn');
    
    switch (state) {
        case 'connecting':
            statusContent.innerHTML = `
                <div class="flex items-center gap-3">
                    <span class="spinner"></span>
                    <div>
                        <p class="font-semibold">Connecting to ${ssid}...</p>
                        <p class="text-sm text-slate-400">Please wait</p>
                    </div>
                </div>
            `;
            statusBadge.textContent = 'Connecting...';
            statusBadge.className = 'badge badge-warning';
            break;
            
        case 'connected':
            statusContent.innerHTML = `
                <div class="flex items-center gap-3">
                    <div class="text-4xl">✅</div>
                    <div>
                        <p class="font-semibold text-green-400">Connected!</p>
                        <p class="text-sm">SSID: ${ssid}</p>
                        <p class="text-sm">IP: ${ip}</p>
                    </div>
                </div>
            `;
            statusBadge.textContent = 'Connected';
            statusBadge.className = 'badge badge-success';
            saveBtn.disabled = false;
            saveBtn.innerHTML = '💾 Save & Connect';
            
            setTimeout(() => {
                if (confirm('Connected! Redirect to device dashboard?')) {
                    window.location.href = `http://${ip}/`;
                }
            }, 3000);
            break;
            
        case 'failed':
            statusContent.innerHTML = `
                <div class="flex items-center gap-3">
                    <div class="text-4xl">❌</div>
                    <div>
                        <p class="font-semibold text-red-400">Connection Failed</p>
                        <p class="text-sm">${message || 'Check credentials and try again'}</p>
                    </div>
                </div>
            `;
            statusBadge.textContent = 'Failed';
            statusBadge.className = 'badge badge-error';
            saveBtn.disabled = false;
            saveBtn.innerHTML = '💾 Save & Connect';
            break;
    }
}

// Load diagnostics
async function loadDiagnostics() {
    try {
        const response = await fetch('/api/diagnostics');
        const diag = await response.json();
        
        const content = document.getElementById('diag-content');
        content.innerHTML = `
            <div class="grid grid-cols-2 gap-4">
                <div>
                    <p class="text-slate-400">Heap Free</p>
                    <p class="font-semibold">${formatBytes(diag.heapFree)}</p>
                </div>
                <div>
                    <p class="text-slate-400">Uptime</p>
                    <p class="font-semibold">${formatUptime(diag.uptime)}</p>
                </div>
                <div>
                    <p class="text-slate-400">WiFi RSSI</p>
                    <p class="font-semibold">${diag.rssi} dBm</p>
                </div>
                <div>
                    <p class="text-slate-400">API Requests</p>
                    <p class="font-semibold">${diag.apiRequests}</p>
                </div>
            </div>
        `;
        
    } catch (error) {
        console.error('Failed to load diagnostics:', error);
    }
}

// Toggle diagnostics panel
function toggleDiagnostics() {
    const content = document.getElementById('diag-content');
    const arrow = document.getElementById('diag-arrow');
    
    content.classList.toggle('hidden');
    arrow.textContent = content.classList.contains('hidden') ? '▼' : '▲';
    
    if (!content.classList.contains('hidden')) {
        loadDiagnostics();
    }
}

// Show toast notification
function showToast(message, type = 'info') {
    const container = document.getElementById('toast-container');
    const toast = document.createElement('div');
    
    const colors = {
        success: 'bg-green-500',
        error: 'bg-red-500',
        warning: 'bg-yellow-500',
        info: 'bg-blue-500'
    };
    
    toast.className = `${colors[type]} text-white px-4 py-3 rounded-lg shadow-lg fade-in`;
    toast.textContent = message;
    
    container.appendChild(toast);
    
    setTimeout(() => {
        toast.style.opacity = '0';
        setTimeout(() => toast.remove(), 300);
    }, 3000);
}

// Utility functions
function formatBytes(bytes) {
    if (bytes < 1024) return bytes + ' B';
    if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(1) + ' KB';
    return (bytes / (1024 * 1024)).toFixed(1) + ' MB';
}

function formatUptime(seconds) {
    if (seconds < 60) return seconds + 's';
    if (seconds < 3600) return Math.floor(seconds / 60) + 'm';
    if (seconds < 86400) return Math.floor(seconds / 3600) + 'h';
    return Math.floor(seconds / 86400) + 'd';
}


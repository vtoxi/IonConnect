#ifndef EMBEDDED_HTML_MINIMAL_H
#define EMBEDDED_HTML_MINIMAL_H

// Minimal HTML for low-memory devices (ESP-01, etc.) - Optimized to <6KB
// Removes: SSE, diagnostics, backup/restore, fancy animations, Tailwind CSS
// Note: This file is included within the IonConnect namespace, so don't declare namespace here
const char EMBEDDED_HTML_MINIMAL[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>IonConnect</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:Arial,sans-serif;background:#0f172a;color:#e2e8f0;padding:10px}
.container{max-width:600px;margin:0 auto}
h1{font-size:20px;margin:15px 0;border-bottom:2px solid #334155;padding-bottom:10px}
h2{font-size:16px;margin:10px 0}
.panel{background:#1e293b;border:1px solid #334155;border-radius:8px;padding:15px;margin:10px 0}
input,select{width:100%;padding:8px;margin:5px 0;background:#334155;border:1px solid #475569;color:#e2e8f0;border-radius:4px}
button{padding:10px;margin:5px;background:#3b82f6;color:#fff;border:none;border-radius:4px;cursor:pointer;font-size:14px}
button:hover{background:#2563eb}
button:disabled{background:#475569;cursor:not-allowed}
.btn-scan{background:#10b981}
.btn-scan:hover{background:#059669}
.btn-clear{background:#ef4444}
.btn-clear:hover{background:#dc2626}
.network{padding:10px;margin:5px 0;background:#334155;border-radius:4px;cursor:pointer;border:1px solid #475569}
.network:hover{background:#475569}
.status{padding:10px;margin:10px 0;background:#1e293b;border-left:4px solid #3b82f6;border-radius:4px}
.status.success{border-color:#10b981}
.status.error{border-color:#ef4444}
label{display:block;margin:8px 0 4px;font-size:14px}
.required::after{content:" *";color:#ef4444}
.hidden{display:none}
.flex{display:flex;gap:5px}
</style>
</head>
<body>
<div class="container">
<h1>🔌 IonConnect</h1>
<div id="status" class="status hidden"></div>
<div class="panel">
<h2>WiFi Networks</h2>
<button onclick="scanNetworks()" class="btn-scan" id="scanBtn">📡 Scan</button>
<div id="networks"></div>
</div>
<div class="panel">
<h2>Configuration</h2>
<form id="configForm" onsubmit="saveConfig(event)"></form>
<div class="flex">
<button onclick="saveConfig()" style="flex:1">💾 Save</button>
<button onclick="clearConfig()" class="btn-clear">🗑️ Clear</button>
</div>
</div>
</div>
<script>
let schema=null;
async function init(){
try{
const r=await fetch('/api/schema');
schema=await r.json();
buildForm();
loadConfig();
}catch(e){showStatus('Failed to load',true);}
}
function buildForm(){
const form=document.getElementById('configForm');
let html='';
schema.fields.forEach(f=>{
const req=f.required?' required':'';
const reqClass=f.required?' class="required"':'';
html+=`<label${reqClass}>${f.label}</label>`;
if(f.type==='select'){
html+=`<select name="${f.id}"${req}>`;
if(f.options)f.options.forEach(o=>html+=`<option>${o}</option>`);
html+='</select>';
}else if(f.type==='checkbox'){
html+=`<input type="checkbox" name="${f.id}">`;
}else if(f.type==='textarea'){
html+=`<textarea name="${f.id}" rows="3"${req}></textarea>`;
}else{
html+=`<input type="${f.type}" name="${f.id}" placeholder="${f.placeholder||''}"${req}>`;
}
});
form.innerHTML=html;
}
async function loadConfig(){
try{
const r=await fetch('/api/config');
const cfg=await r.json();
Object.keys(cfg).forEach(k=>{
const inp=document.querySelector(`[name="${k}"]`);
if(inp){
if(inp.type==='checkbox')inp.checked=cfg[k]==='true';
else inp.value=cfg[k];
}
});
}catch(e){}
}
async function scanNetworks(){
const btn=document.getElementById('scanBtn');
btn.disabled=true;
btn.textContent='Scanning...';
try{
await fetch('/api/scan',{method:'POST'});
await new Promise(r=>setTimeout(r,3000));
const r=await fetch('/api/networks');
const data=await r.json();
displayNetworks(data.networks||[]);
}catch(e){
showStatus('Scan failed',true);
}finally{
btn.disabled=false;
btn.textContent='📡 Scan';
}
}
function displayNetworks(nets){
const div=document.getElementById('networks');
if(nets.length===0){
div.innerHTML='<p>No networks found</p>';
return;
}
div.innerHTML=nets.map(n=>{
const lock=n.encryption!==0?'🔒':'';
const sig=n.rssi>-60?'📶':n.rssi>-70?'📡':'📡';
return`<div class="network" onclick="selectNet('${n.ssid}')">${sig} ${n.ssid} ${lock} (${n.rssi}dBm)</div>`;
}).join('');
}
function selectNet(ssid){
const inp=document.querySelector('[name="wifi_ssid"]');
if(inp){
inp.value=ssid;
const pass=document.querySelector('[name="wifi_pass"]');
if(pass)pass.focus();
}
}
async function saveConfig(e){
if(e)e.preventDefault();
const form=document.getElementById('configForm');
const data=new FormData(form);
const cfg={};
data.forEach((v,k)=>{
const inp=form.querySelector(`[name="${k}"]`);
cfg[k]=inp.type==='checkbox'?inp.checked?'true':'false':v;
});
try{
const r=await fetch('/api/config',{
method:'POST',
headers:{'Content-Type':'application/json'},
body:JSON.stringify(cfg)
});
if(r.ok){
showStatus('Saved! Connecting...',false);
setTimeout(()=>location.reload(),3000);
}else{
showStatus('Save failed',true);
}
}catch(e){
showStatus('Save failed',true);
}
}
async function clearConfig(){
if(!confirm('Clear all config?'))return;
try{
const r=await fetch('/api/clear',{method:'POST'});
if(r.ok){
showStatus('Cleared',false);
setTimeout(()=>location.reload(),1500);
}else{
showStatus('Clear failed',true);
}
}catch(e){
showStatus('Clear failed',true);
}
}
function showStatus(msg,isError){
const s=document.getElementById('status');
s.textContent=msg;
s.className='status '+(isError?'error':'success');
s.classList.remove('hidden');
setTimeout(()=>s.classList.add('hidden'),3000);
}
init();
</script>
</body>
</html>
)rawliteral";

// For compatibility - minimal mode uses inline JS
const char EMBEDDED_JS_MINIMAL[] PROGMEM = "";

#endif // EMBEDDED_HTML_MINIMAL_H


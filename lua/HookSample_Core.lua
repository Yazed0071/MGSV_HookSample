local this = {}

if rawget(_G, "HookSample_Core") then
    return _G.HookSample_Core
end

local ok, hookSampleOrErr = pcall(require, "HookSample")
if not ok then
    error("HookSample_Core: failed to require HookSample: " .. tostring(hookSampleOrErr))
end

local HS = hookSampleOrErr

_G.HS = HS
_G.HookSample = HS
_G.HookSample_Core = this

this.HookSample = HS

return this
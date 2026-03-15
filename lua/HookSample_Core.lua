local this = {}

if rawget(_G, "HookSample_Core") then
    return _G.HookSample_Core
end

local ok, hookSampleOrErr = pcall(require, "HookSample")
if not ok then
    error("HookSample_Core: failed to require HookSample: " .. tostring(hookSampleOrErr))
end

local VFW = hookSampleOrErr

_G.VFW = VFW
_G.HookSample = VFW
_G.HookSample_Core = this

this.HookSample = VFW

return this
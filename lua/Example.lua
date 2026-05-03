local this = {}

-- Equip icon override: replace an equip's icon ftex by equipId.
function this.SetEquipIcon(equipId, ftexPath)
    HookSample.SetEquipIconFtexPath(equipId, ftexPath)
end

function this.ClearEquipIcon(equipId)
    HookSample.ClearEquipIconFtexPath(equipId)
end

function this.ClearAllEquipIcons()
    HookSample.ClearAllEquipIconFtexPaths()
end

return this

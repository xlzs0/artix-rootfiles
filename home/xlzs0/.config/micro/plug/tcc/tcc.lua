VERSION = "0.0.1"

function onBufferOpen(buf)
	if buf:Line(0):match("^#!.*tcc") then
		buf:SetOptionNative("filetype", "c")
	end
end

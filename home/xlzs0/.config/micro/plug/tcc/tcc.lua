VERSION = "0.0.1"

function onBufferOpen(buf)
	if buf:Line(0):match("^#!.*tcc") or buf:Line(2):match("^exec tcc") then
		buf:SetOptionNative("filetype", "c")
	end
end

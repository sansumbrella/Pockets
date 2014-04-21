-- How can I encapsulate the contents of this file
-- into a structure that would allow me to have multiple
-- instances of the same script running in the same
-- lua_State on the C side?

value = 0.0

function setup()
	value = 0.0
	su.cout( "Setting up script" )
end

function update( dt )
	value = value + dt
	print( "Elapsed time: " .. value )
	do_something()
end

-- This seems like a promising start for creating
-- multiple variables with the same base behavior,
-- but I still need a way to capture a unique variable
-- from it.
function LuaScript()
	local t
	local obj = {}

	function obj:setup()
		t = 0.0
	end

	function obj:update( dt )
		t = t + dt
		-- su.cout( "Elapsed time: " .. t )
		print( "Elapsed time: " .. t )
	end

	return obj
end

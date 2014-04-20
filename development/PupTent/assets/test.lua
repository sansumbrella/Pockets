value = 0.0

function setup()
	value = 0.0
	su.cout( "Setting up script" )
end

function update( dt )
	value = value + dt
	su.cout( "Elapsed time: " .. value )
end

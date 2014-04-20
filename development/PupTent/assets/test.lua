value = 0.0

function update( dt )
	value = value + dt
	su.cout( "Elapsed time: " .. value )
end

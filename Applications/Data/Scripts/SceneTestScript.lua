
dofile( "../Data/Scripts/Actor.lua" )




App.Log( "Outside" );

function Initialize()

	handle = 0;
	App.Log( "Handle Value: " .. handle );

	handle = App.CreateActor( "box.ms3d", "Phong" );
	a1 = Actor:Create( handle );

	App.Log( "Handle Value: " .. handle );

end



function Update( time )
	--App.Log( tostring( time ) );
end


function Render()

end

--

function Shutdown()
	App.Log( "Shutdown" );
end


function OnKeyDown( char )

	if char == 49 then
		handle = App.CreateActor( "box.ms3d", "Phong" );
		local a = Actor:Create( handle );
		a:SetPosition( math.random( -10, 10 ), math.random( -10, 10 ), math.random( -10, 10 ) );
		a:SetRotation( math.random( 10 ), math.random( 10 ), math.random( 10 ) );
		
		a1:AttachChild( a );
	end

	if char == 50 then -- '2' Key
	end
	
	if char == 51 then
	end

	if char == 52 then
	end
	
	if char == 53 then
	end

end


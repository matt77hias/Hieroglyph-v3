

App.Log( "The script has been processed..." );

--------------------------------------------------------------------------------
function Initialize()

	--App.Log( "Initialization function has been called..." );

	myactor = GeometryActor.new();
	textActor = TextActor.new();
	GlyphScene:AddActor( myactor );
	GlyphScene:AddActor( textActor );
	
	textActor:SetText( "I can write text from here now bizatches!" );
	
	local s = 5;
	local rand = math.random;
	
	for i = 1, 2 do
		myactor:SetColor( rand( 0.0, 1.0 ), rand( 0.0, 1.0 ), rand( 0.0, 1.0 ), 1.0 );
		myactor:DrawSphere( rand( -s, s ), rand( -s, s ), rand( -s, s ), 1.0 );
	end

	for i = 1, 10 do
		myactor:SetColor( 1.0, 0.0, 0.0, 1.0 );
		myactor:DrawDisc( 
			2*i-10, 5.0, 5.0,  -- center
			0.0, 1.0, -1.0, -- normal
			0.5+0.1*i		-- radius
		);
	end

	for i = 1, 10 do
		myactor:SetColor( 0.0, 1.0, 0.0, 1.0 );
		myactor:DrawCylinder( 
			2*i-10, 3.0, 5.0,  -- p1
			2*i-10, 1.0, 5.0,  -- p1
			0.5+0.1*i,		-- r1
			1.0-0.1*i		-- r2
		);
	end

	for i = 1, 10 do
		myactor:SetColor( 0.0, 0.0, 1.0, 1.0 );
		myactor:DrawBox( 
			2*i-10, -1.0, 3.0,  -- p1
			1, 0, 0, -- xdir
			0, 1, 0, -- ydir
			0, 0, 1, -- zdir
			i*0.1, 1.2-i*0.1, 1.0 -- extents
		);
	end	
	
	for i = 1, 10 do
		myactor:SetColor( 1.0, 1.0, 0.0, 1.0 );
		myactor:DrawRect( 
			2*i-10, -3.0, 3.0,  -- center
			-1, 0, 0, -- xdir
			0, 1, 0, -- ydir
			i*0.1, 1.2-i*0.1 -- extents
		);
	end		
end
--------------------------------------------------------------------------------
function Update( time )
	--App.Log( tostring( App.Framerate() ) );
	--App.Log( tostring( time ) );
end
--------------------------------------------------------------------------------
function Shutdown()
	--App.Log( "Shutdown function has been called..." );
end
--------------------------------------------------------------------------------
function OnKeyDown( char )

	if char == 49 then
	end

	if char == 50 then -- '2' Key
	end
	
	if char == 51 then
	end

	if char == 52 then
	end
	
	if char == 53 then -- '5' Key
		local s = 5;
		myactor:DrawSphere( math.random( -s, s ), math.random( -s, s ), math.random( -s, s ), 1.0 );
	end

	if char == 54 then -- '6' Key
		textActor:SetText( 'Text from the script!' );
		textActor:SetFont( 'Consolas', 20, 0, 1 );
	end

	if char == 55 then -- '7' Key
		textActor:SetText( 'Text from the script!' );
		textActor:SetFont( 'Consolas', 30, 0, 0 );
	end
	
end
--------------------------------------------------------------------------------

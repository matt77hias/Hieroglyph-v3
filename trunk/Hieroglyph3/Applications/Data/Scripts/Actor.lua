--dofile( "Data/Scripts/CompositeShape.lua" );

function Actor:GetPosition()
	return( self._GetPosition( self.handle ) );
end

function Actor:SetPosition( x, y, z )
	self._SetPosition( self.handle, x, y, z );
end

function Actor:SetRotation( x, y, z )
	self._SetRotation( self.handle, x, y, z );
end

function Actor:Create( handle )
	local o = {};					-- Create the object's table
	setmetatable( o, self );		-- Set the object's metatable
	self.__index = self;			-- Set the object's index table
	o.handle = handle;				-- Create the engine version of object
	return o;
end

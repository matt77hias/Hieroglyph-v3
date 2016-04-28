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

function Actor:AttachChild( a )
	self._AttachChild( self.handle, a.handle );
end

function Actor:DetachChild( a )
	self._DetachChild( self.handle, a.handle );
end

function Actor:SetEntityVectorParameter( name, x, y, z, w )
	self._SetEntityVectorParameter( self.handle, name, x, y, z, w );
end

function Actor:SetMaterialVectorParameter( name, x, y, z, w )
	self._SetMaterialVectorParameter( self.handle, name, x, y, z, w );
end


function Actor:Create( handle )
	local o = {};					-- Create the object's table
	setmetatable( o, self );		-- Set the object's metatable
	self.__index = self;			-- Set the object's index table
	o.handle = handle;				-- Create the engine version of object
	return o;
end

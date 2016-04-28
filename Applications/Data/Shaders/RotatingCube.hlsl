//-----------------------------------------------------------------------------
cbuffer Transforms
 {
	matrix WorldViewProjMatrix;	
};

//-----------------------------------------------------------------------------
struct VS_INPUT
{
	float4 position : SV_Position;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float4 color : COLOR;
};

struct GS_INPUTOUTPUT
{
	float4 position			: SV_Position;
	float4 color			: COLOR;
};

//-----------------------------------------------------------------------------
VS_OUTPUT VSMain( in VS_INPUT v )
{
	VS_OUTPUT o = (VS_OUTPUT)0;

	// Simply propogate attributes through
	o.position = v.position;
	o.color = v.color;

	return o;
}

[maxvertexcount(3)]
void GSMain( triangle GS_INPUTOUTPUT input[3], inout TriangleStream<GS_INPUTOUTPUT> TriangleOutputStream )
{
	GS_INPUTOUTPUT output;

	// Calculate the face normal
	float3 faceEdgeA = input[1].position.xyz - input[0].position.xyz;
	float3 faceEdgeB = input[2].position.xyz - input[0].position.xyz;
	float3 faceNormal = normalize( cross(faceEdgeA, faceEdgeB) );

	// Output vertices
	for (int i = 0; i < 3; i++) {
		// Blow up the cube
		float4 position = input[i].position + float4(faceNormal/4, 0);
		
		// Transform the new position to clipspace.
		output.position = mul(position, WorldViewProjMatrix);

		output.color = input[i].color;

		TriangleOutputStream.Append(output);
	}
}


//-----------------------------------------------------------------------------
float4 PSMain( in GS_INPUTOUTPUT input ) : SV_Target
{
	float4 color = input.color;

	return( color );
}
//-----------------------------------------------------------------------------
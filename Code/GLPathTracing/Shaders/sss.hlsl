
struct pixOut
{
	float4 normal		: SV_TARGET0;	//xyz = normal, w = ?
	float4 diff			: SV_TARGET1;	//xyz = diffuse, w = ?
	float4 tranclucent	: SV_TARGET2;	//xyz = translucent, w = ?
};

//[numthreads(1, 1, 1)]
//void CSMainIncrement( uint3 threadID : SV_DispatchThreadID )
//{

pixOut main()
{
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
	pixOut o = (pixOut)0;
	o.diff = float4(1.0, 1.0, 0.0, 1.0);
	return o;
}
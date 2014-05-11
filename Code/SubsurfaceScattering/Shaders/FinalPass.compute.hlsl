#include "FinalPass.header.hlsli"



[numthreads(32, 32, 1)]
void main(	uint3 dti	: SV_DispatchThreadID, 
			uint3 gti	: SV_GroupThreadID, 
			uint3 gid	: SV_GroupID, 
			uint gi		: SV_GroupIndex )
{
	float4 color		= gSRVColor[dti.xy];
	float4 light		= gLightMap[dti.xy];
	//float4 thickness	= gThicknessMap[dti.xy];
	//float4 normal		= gSRVNormal[dti.xy];
	//float depth			= gShadowMap[dti.xy].r;

	//float sh = 0.0f;
	//for (int i = 0; i < LightMatrix.Length; i++)
	//{
	//	sh += = CalcShadow(float4(lightMatrix));
	//}

	// float4 lDotN = dot(LightDirection, normal);
	//float lDotN = dot( float4(0, 0, 1, 1), normal );

	FinalImage[dti.xy] = (saturate(color) * saturate(light));
	//FinalImage[dti.xy] = saturate(color + (saturate(light) * lDotN));
	//FinalImage[dti.xy] = normal;
	//FinalImage[dti.xy] = thickness;
}




/*
half3 LeafShadingBack(	half3 vEye,
						half3 vLight,
						half3 vNormal,
						half3 cDiffBackK,
						half fBackViewDep)
{
	half fEdotL = saturate(dot(vEye.xyz, -vLight.xyz));
	half fPowEdotL = fEdotL * fEdotL;
	fPowEdotL *= fPowEdotL;

	// Back diffuse shading, wrapped slightly
	half fLdotNBack = saturate(dot(-vNormal.xyz, vLight.xyz)*0.6 + 0.4);

	// Allow artists to tweak view dependency.
	half3 vBackShading = lerp(fPowEdotL, fLdotNBack, fBackViewDep);

	// Apply material back diffuse color.
	return vBackShading * cDiffBackK.xyz;
}

void LeafShadingFront(	half3 vEye,
						half3 vLight,
						half3 vNormal,
						half3 cDifK,
						half4 cSpecK,
						out half3 outDif,
						out half3 outSpec)
{
	half fLdotN = saturate(dot(vNormal.xyz, vLight.xyz));

	outDif = fLdotN * cDifK.xyz;
	outSpec = Phong(vEye, vLight, cSpecK.w) * cSpecK.xyz;
}
void frag_custom_per_light(inout fragPass pPass, inout fragLightPass pLight)
{
	half3 cDiffuse = 0, cSpecular = 0;
		
	LeafShadingFront(pPass.vReflVec, pLight.vLight, pPass.vNormal.xyz, pLight.cDiffuse.xyz, pLight.cSpecular, cDiffuse, cSpecular);

	// Shadows * light falloff * light projected texture
	half3 cK = pLight.fOcclShadow * pLight.fFallOff * pLight.cFilter;
		
	// Accumulate results.
	pPass.cDiffuseAcc += cDiffuse * cK;
	pPass.cSpecularAcc += cSpecular * cK;
	pPass.pCustom.fOcclShadowAcc += pLight.fOcclShadow;
}

void frag_custom_ambient(inout fragPass pPass, inout half3 cAmbient)
{
	// Hemisphere lighting approximation
	cAmbient.xyz = lerp(cAmbient*0.5f, cAmbient,
		saturate(pPass.vNormal.z*0.5f + 0.5f));
	pPass.cAmbientAcc.xyz = cAmbient;
}

void frag_custom_end(inout fragPass pPass, inout half3 cFinal)
{
	if (pPass.nlightCount && pPass.pCustom.bLeaves) 
	{
		// Normalize shadow accumulation.
		half fOccFactor = pPass.pCustom.fOcclShadowAcc / pPass.nlightCount;
		
		// Apply subsurface map.
		pPass.pCustom.cShadingBack.xyz *= pPass.pCustom.cBackDiffuseMap;
		
		// Apply shadows and light projected texture.
		pPass.pCustom.cShadingBack.xyz *= fOccFactor * pPass.pCustom.cFilterColor;
	}

	// Apply diffuse texture and material diffuse color to ambient/diffuse/sss terms.
	cFinal.xyz = (pPass.cAmbientAcc.xyz + pPass.cDiffuseAcc.xyz + pPass.pCustom.cShadingBack.xyz) * pPass.cDiffuseMap.xyz * MatDifColor.xyz;
	
	// Apply gloss map and material specular color, add to result.
	cFinal.xyz += pPass.cSpecularAcc.xyz * pPass.cGlossMap.xyz * MatSpecColor.xyz;
	
	// Apply prebaked ambient occlusion term.
	cFinal.xyz *= pPass.pCustom.fAmbientOcclusion;
}

*/
#ifndef INCLUDEGUARD_MATERIAL_H
#define INCLUDEGUARD_MATERIAL_H

#include <D3DTK\SimpleMath.h>

namespace MaterialLayers
{
	static const DirectX::SimpleMath::Vector4 SkinLayers[] = 
	{
		DirectX::SimpleMath::Vector4( 0.233f, 0.455f, 0.649f, 0.0064f),
		DirectX::SimpleMath::Vector4( 0.100f, 0.336f, 0.344f, 0.0484f),
		DirectX::SimpleMath::Vector4( 0.118f, 0.198f, 0.000f, 0.1870f),
		DirectX::SimpleMath::Vector4( 0.113f, 0.007f, 0.007f, 0.5670f),
		DirectX::SimpleMath::Vector4( 0.358f, 0.004f, 0.000f, 1.9900f),
		DirectX::SimpleMath::Vector4( 0.078f, 0.000f, 0.000f, 7.4100f),
	};

	static const DirectX::SimpleMath::Vector4 PigLayers[] = 
	{
		DirectX::SimpleMath::Vector4( 0.113f, 0.007f, 0.007f, 0.5670f),
		DirectX::SimpleMath::Vector4( 0.358f, 0.004f, 0.358f, 1.9900f),
		DirectX::SimpleMath::Vector4( 0.078f, 0.000f, 0.113f, 7.4100f),
	};

	static const DirectX::SimpleMath::Vector4 Vegetation[] =
	{
		DirectX::SimpleMath::Vector4(0.5f, 085.0f, 07.0f, 7.4f),
	};
	static const DirectX::SimpleMath::Vector4 SingleRedLayer[] = 
	{
		DirectX::SimpleMath::Vector4( 0.78f, 0.0f, 0.0f, 7.4100f),
	};
	static const DirectX::SimpleMath::Vector4 SingleBlueLayer[] = 
	{
		DirectX::SimpleMath::Vector4( 0.0f, 0.28f, 0.0f, 3.4100f),
	};
	static const DirectX::SimpleMath::Vector4 SingleGreenLayer[] = 
	{
		DirectX::SimpleMath::Vector4( 0.0f, 0.0f, 0.78f, 7.4100f),
	};

	static const DirectX::SimpleMath::Vector4 Material8Layer[] = 
	{
		DirectX::SimpleMath::Vector4( 0.233f, 0.455f, 0.649f, 0.0064f),
		DirectX::SimpleMath::Vector4( 0.100f, 0.336f, 0.344f, 0.0484f),
		DirectX::SimpleMath::Vector4( 0.118f, 0.198f, 0.000f, 0.1870f),
		DirectX::SimpleMath::Vector4( 0.113f, 0.007f, 0.007f, 0.5670f),
		DirectX::SimpleMath::Vector4( 0.358f, 0.004f, 0.000f, 0.9900f),
		DirectX::SimpleMath::Vector4( 0.078f, 0.000f, 0.000f, 1.1100f),
		DirectX::SimpleMath::Vector4( 0.078f, 0.030f, 0.030f, 1.2100f),
		DirectX::SimpleMath::Vector4( 0.078f, 0.030f, 0.030f, 1.7100f),
	};
	static const DirectX::SimpleMath::Vector4 Material10Layer[] = 
	{
		DirectX::SimpleMath::Vector4( 0.233f, 0.455f, 0.649f, 0.0064f),
		DirectX::SimpleMath::Vector4( 0.100f, 0.336f, 0.344f, 0.0484f),
		DirectX::SimpleMath::Vector4( 0.118f, 0.198f, 0.000f, 0.1870f),
		DirectX::SimpleMath::Vector4( 0.113f, 0.007f, 0.007f, 0.5670f),
		DirectX::SimpleMath::Vector4( 0.358f, 0.004f, 0.000f, 0.9900f),
		DirectX::SimpleMath::Vector4( 0.078f, 0.000f, 0.358f, 1.1100f),
		DirectX::SimpleMath::Vector4( 0.078f, 0.00f, 0.030f, 1.2100f),
		DirectX::SimpleMath::Vector4( 0.078f, 0.00f, 0.030f, 1.7100f),
		DirectX::SimpleMath::Vector4( 0.078f, 0.030f, 0.030f, 2.7100f),
		DirectX::SimpleMath::Vector4( 0.078f, 0.358f, 0.0f, 3.1100f),
	};

	static const DirectX::SimpleMath::Vector4* LayerArray[] =
	{
		{ SingleRedLayer },
		{ SingleGreenLayer },
		{ SingleBlueLayer },
		{ PigLayers },
		{ Material8Layer },
		{ SkinLayers },
		{ Material10Layer },
	};
}

#endif // !INCLUDEGUARD_MATERIAL_H

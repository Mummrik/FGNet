#pragma once

#include "Blueprint/UserWidget.h"
#include "FGNetDebugWidget.generated.h"

USTRUCT(BlueprintType)
struct FFGBlueprintNetworkSimulationSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Network Settings", meta = (DisplayName = "Minimum Latency", ClampMin = "0", ClampMax = "5000"))
		int32 MinLatency = 0;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Network Settings", meta = (DisplayName = "Maximum Latency", ClampMin = "0", ClampMax = "5000"))
		int32 MaxLatency = 0;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Network Settings", meta = (ClampMin = "0", ClampMax = "100"))
		int32 PacketLossPercentage = 0;
};

USTRUCT(BlueprintType)
struct FFGBlueprintNetworkSimulationSettingsText
{
	GENERATED_BODY()
public:
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Network Settings", meta = (DisplayName = "Minimum Latency"))
		FText MinLatency;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Network Settings", meta = (DisplayName = "Maximum Latency"))
		FText MaxLatency;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Network Settings", meta = (ClampMin = "0", ClampMax = "100"))
		FText PacketLossPercentage;
};

UCLASS()
class FGNET_API UFGNetDebugWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	//UFGNetDebugWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = Widget)
		void UpdateNetworkSimulationSettings(const FFGBlueprintNetworkSimulationSettings& inPackets);

	UFUNCTION(BlueprintImplementableEvent, Category = Widget, meta = (DisplayName = "On Update Network Simulation Settings"))
		void BP_OnUpdateNetworkSimulationSettings(const FFGBlueprintNetworkSimulationSettingsText& Packets);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = Widget, meta = (DisplayName = "On Update Ping"))
		void BP_UpdatePing(int32 Ping);

	UFUNCTION(BlueprintImplementableEvent, Category = Widget, meta = (DisplayName = "On Show Widget"))
		void BP_OnShowWidget();

	UFUNCTION(BlueprintImplementableEvent, Category = Widget, meta = (DisplayName = "On Hide Widget"))
		void BP_OnHideWidget();
};
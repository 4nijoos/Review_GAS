// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TestCharacter.h"
#include "AbilitySystemComponent.h"
#include "Components/WidgetComponent.h"
#include "GameAbilitySystem/AttributeSet/PlayerAttributeSet.h"
#include "GameAbilitySystem/AttributeSet/EnemyAttributeSet.h"
#include "EnhancedInputComponent.h"

#include "UI/BarWidget.h"

AGasCharacter::AGasCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// 요구사항에 맞춰 두 AttributeSet 모두 생성 (하나의 캐릭터 클래스로 둘 다 쓰기 위해)
	PlayerAttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("PlayerAttributeSet"));
	EnemyAttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("EnemyAttributeSet"));

	HealthWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidgetComp"));
	HealthWidgetComp->SetupAttachment(RootComponent);
}

void AGasCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		// 기본 스탯 적용
		if (DefaultAttributeEffect)
		{
			AbilitySystemComponent->ApplyGameplayEffectToSelf(DefaultAttributeEffect.GetDefaultObject(), 1.0f, AbilitySystemComponent->MakeEffectContext());
		}

		// 파이어볼 어빌리티 부여 (플레이어인 경우만 의미 있음)
		if (FireballAbilityClass)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(FireballAbilityClass, 1, 0));
		}

		// 값 변경 감지 (UI 연결)
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetMaxManaAttribute()).AddUObject(this, &AGasCharacter::OnMaxManaChange);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetManaAttribute()).AddUObject(this, &AGasCharacter::OnManaChange);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UEnemyAttributeSet::GetHealthAttribute()).AddUObject(this, &AGasCharacter::OnHealthChange);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UEnemyAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &AGasCharacter::OnMaxHealthChange);
		
		if (HealthWidgetComp)
		{
			UBarWidget* EnemyBar = Cast<UBarWidget>(HealthWidgetComp->GetUserWidgetObject());
			if (EnemyBar && EnemyAttributeSet)
			{
				// 시작하자마자 100/100으로 UI 세팅
				UE_LOG(LogTemp, Warning, TEXT("성공: 적 체력바 위젯을 찾았습니다!"));
				EnemyBar->UpdateMax_Implementation(EnemyAttributeSet->GetMaxHealth());
				EnemyBar->UpdateCurrent_Implementation(EnemyAttributeSet->GetHealth());
			}
			else
			{
				// 이게 뜨면 100% 위젯 부모 클래스 문제입니다.
				UE_LOG(LogTemp, Error, TEXT("실패: 적 체력바가 UBarWidget이 아닙니다! 부모 클래스를 확인하세요!"));
			}
		}
	}
}

void AGasCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (IA_Fire)
		{
			EnhancedInput->BindAction(IA_Fire, ETriggerEvent::Started, this, &AGasCharacter::OnFireballInput);
		}
	}
}

void AGasCharacter::OnFireballInput()
{
	if (AbilitySystemComponent && FireballAbilityClass)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(FireballAbilityClass);
	}
}

void AGasCharacter::OnManaChange(const FOnAttributeChangeData& InData)
{
	if (PlayerHUDWidget)
	{
		// 정답: 현재값(Current)을 바꿔야 함
		PlayerHUDWidget->UpdateCurrent_Implementation(InData.NewValue);
	}
}

// 플레이어 최대 마나 변경 시
void AGasCharacter::OnMaxManaChange(const FOnAttributeChangeData& InData)
{
	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->UpdateMax_Implementation(InData.NewValue);
	}
}

//적 체력 변경 시 (Current)
void AGasCharacter::OnHealthChange(const FOnAttributeChangeData& InData)
{
	// 블루프린트 위젯 업데이트 로직 (Enemy HP Bar)
	if (HealthWidgetComp)
	{
		// 위젯 컴포넌트에서 위젯 가져와서 캐스팅
		UBarWidget* EnemyBar = Cast<UBarWidget>(HealthWidgetComp->GetUserWidgetObject());
		if (EnemyBar)
		{
			EnemyBar->UpdateCurrent_Implementation(InData.NewValue);
		}
	}
}

// 적 체력 최대치 변경 시 (Max)
void AGasCharacter::OnMaxHealthChange(const FOnAttributeChangeData& InData)
{
	if (HealthWidgetComp)
	{
		UBarWidget* EnemyBar = Cast<UBarWidget>(HealthWidgetComp->GetUserWidgetObject());
		if (EnemyBar)
		{
			EnemyBar->UpdateMax_Implementation(InData.NewValue);
		}
	}
}

//플레리어 HUD 위젯 설정 함수
void AGasCharacter::SetHUDWidget(UBarWidget* InWidget)
{
	PlayerHUDWidget = InWidget;

	// 위젯이 연결되자마자 현재 마나/최대 마나를 UI에 한번 쏴줍니다.
	if (PlayerHUDWidget && PlayerAttributeSet)
	{
		PlayerHUDWidget->UpdateMax_Implementation(PlayerAttributeSet->GetMaxMana());
		PlayerHUDWidget->UpdateCurrent_Implementation(PlayerAttributeSet->GetMana());
	}
}

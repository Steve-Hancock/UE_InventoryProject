#include "Items/Inv_ItemTags.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Axe);

namespace GameItems
{
	namespace Equipment
	{
		namespace Weapons
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Axe, "GameItems.Equipment.Weapons.Axe", "Axe");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sword, "GameItems.Equipment.Weapons.Sword", "Sword");
		}

		namespace Cloaks
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(RedCloak, "GameItems.Equipment.Cloaks.RedCloak", "RedCloak");
		}

		namespace Masks
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(StealMask, "GameItems.Equipment.Masks.StealMask", "StealMask");
		}
	}

	namespace Consumables
	{
		namespace Potions
		{
			namespace Red
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Small, "GameItems.Consumables.Potions.Red.Small", "Small Red Potion");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Large, "GameItems.Consumables.Potions.Red.Large", "Large Red Potion");
			}
			namespace Blue
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Small, "GameItems.Consumables.Potions.Blue.Small", "Small Blue Potion");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Large, "GameItems.Consumables.Potions.Blue.Large", "Large Blue Potion");
			}
		}
	}

	namespace Craftables
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(FireFernFruit, "GameItems.Craftables.FireFernFruit", "Fire Fern Fruit");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LuminDaisy, "GameItems.Craftables.LuminDaisy", "Lumen Daisy");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ScorchedPetalBlossom, "GameItems.Craftables.ScorchedPetalBlossom", "Scorched Petal Blossom");
	}

	namespace Collectables
	{

	}
}

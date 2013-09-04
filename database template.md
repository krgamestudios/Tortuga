## Server

#### UserAccount
* userAccountID primary key
* username unique
* password --stored in the database (hashed)?
* blacklisted {false, true}
* whitelisted {true, false}

## Items

#### Notes
* These are static; they're immutable during runtime

#### ItemList
* itemListID primary key
* itemName unique
* itemImage
* type {mundane, consumable, equipment, etc.}
* stackable {1-max; -1 for false}
* maxUniqueCopies {1-max; -1 for unlimited}

#### MundaneItemList
* itemListID foreign key -> ItemList.itemListID
* TODO: attributes

#### ConsumableItemList
* itemListID foreign key -> ItemList.itemListID
* TODO: attributes

#### EquipmentItemList
* itemListID foreign key -> ItemList.itemListID
* TODO: attributes

## Player

#### Notes
* These change as the character progresses and grows

#### PlayerCharacter
* playerCharacterID primary key
* characterName unique
* TODO: customizable sprite

#### PlayerStatistics
* playerCharacterID foreign key -> PlayerCharacter.playerCharacterID
* currentLevel
* currentExperience
* maxHealth
* maxMana
* currentHealth
* currentMana
* attack
* defence
* TODO: etc.

#### PlayerEquipment
* playerCharacterID foreign key -> PlayerCharacter.playerCharacterID
* head foreign key -> ItemList.itemListID
* weapon foreign key -> ItemList.itemListID
* armour foreign key -> ItemList.itemListID
* TODO: etc.

#### PlayerInventoryItem
* playerCharacterID foreign key -> PlayerCharacter.playerCharacterID
* itemListID foreign key -> ItemList.itemListID

[🇬🇧 English](#english) · [🇷🇺 Русский](#русский)

---

## English

# Descent

![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-4.27.2-black?logo=unrealengine)
![C++](https://img.shields.io/badge/C%2B%2B-70%25-blue?logo=cplusplus)
![Blueprint](https://img.shields.io/badge/Blueprint-AI%20%7C%20UI%20%7C%20Game-purple?logo=unrealengine)
![Multiplayer](https://img.shields.io/badge/Multiplayer-Co--op%20Listen%20Server-green)
![Platform](https://img.shields.io/badge/Platform-PC%20%7C%20Steam%20Deck-lightblue)
![License](https://img.shields.io/badge/License-MIT-lightgrey)

**A top-down co-op shooter built with C++ and Blueprints in Unreal Engine 4.27.2.**

Four weapon classes. An AI-driven enemy roster with a boss. A replicated status-effect system. A full lobby with character customisation. Play solo or drop in a friend — on PC or Steam Deck.

👾 **[Play the demo on itch.io →](https://vl-wolf.itch.io/descent)**

---

### Features

**Combat**
- 4 weapon types — Rifle, Shotgun, Sniper Rifle, Grenade Launcher
- Per-weapon **movement-state dispersion** model (Aim / Walk / Run / Sprint each have independent accuracy curves)
- Grenade fuse timer with configurable radius damage falloff
- Physics-simulated shell casings and magazine drops on every reload
- Surface-dependent hit decals and particle FX

**Enemies & Boss**
- Regular enemies with full Behavior Tree AI: patrol → hear → chase → attack
- **Boss** with a dedicated BT, rocket turrets (`BP_TurretRocketBoss`), and standalone `BP_RocketTurret` actors
- Wave/phase enemy spawner (`BP_EnemySpawn` + `DT_Levels`)

**Status Effects**
- Replicated, Blueprintable `UTDS_StateEffect` with three tiers: instant, damage-over-time, area-of-effect
- Blueprint implementations: Fire DoT, Healing Once, Healing Over Time, AoE
- Surface-type filtered, bone-attached, stackable or replacing

**Inventory & Pickups**
- 4 weapon slots with per-type ammo tracking
- In-world pickup actors: weapon, ammo, first aid kit
- Weapon drop spawns physics prop with impulse

**Multiplayer & Lobby**
- Listen-server co-op — all state server-authoritative and fully replicated
- Lobby with session creation, player readying, and **cape colour customisation** (4 colours)
- In-game HUD: health, weapon slots, ammo, switch overlay, floating damage numbers, kill score

**Environment**
- Medieval dungeon: interactive doors, cell doors, dynamic torch systems (light paths to boss / weapon rooms)
- Input hint icons switch automatically between keyboard and Xbox gamepad layouts
- Full options menu with level selection and server browser

**Platform**
- Windows (Win64) and **Steam Deck / Linux**
- Full **Xbox-compatible gamepad** support (deadzone 0.25 on both sticks)

---

### Architecture

```
C++ Core                          Blueprint Layer
──────────────────────────────    ──────────────────────────────────────────
ATDSCharacter                     BP_Character
  ├ UTDSInventoryComponent           BP_WeaponDefault · Projectile · Grenade
  ├ UTDSCharacterHealthComponent     BP_CharacterEnemy_Base
  └ UTDS_EffectComponent                BT_AI_Base (patrol→hear→chase→attack)
AWeaponDefault                     BP_CharacterEnemy_Boss
  └ AProjectileDefault                  BT_AI_Boss · BP_TurretRocketBoss
      └ AProjectileDefault_Grenade  BP_EnemySpawn
UTDS_StateEffect                   BP_StateEffect_Fire / Healing / AoE
  ├ ExecuteOnce                     BP_PickUpActor_Weapon / _Ammo / _FirstAid
  ├ ExecuteTimer                    BP_DoorSystem · BP_TorchSystem*
  └ AreaOfEffect                    WBP_HUD · BPW_InGameWidget · BPW_Menu
ATDSPlayerController               BP_LobbyMode · BP_LobbyPlayerController
UTDSGameInstance                   DT_WeaponInfo · DT_DropItemInfo · DT_Levels
```

---

### Tech Stack

| | |
|---|---|
| Engine | Unreal Engine 4.27.2 |
| Language | C++ + Blueprints |
| AI | Behavior Tree + Blackboard |
| Networking | Listen Server · Unreal Replication |
| Platforms | Windows (Win64) · Steam Deck / Linux |

---

### Controls

**Keyboard & Mouse**

| Input | Action |
|-------|--------|
| `W A S D` | Move |
| `Mouse` | Aim |
| `LMB / RMB` | Fire / Aim mode |
| `Left Shift / Left Alt` | Sprint / Walk |
| `R` | Reload |
| `Z` | Health ability |
| `1 – 4` | Weapon slot |
| `C / X` | Next / Previous weapon |
| `G` | Drop weapon |
| `E` | Interact / Pick up |
| `Esc` | Menu |

**Gamepad (Xbox, deadzone 0.25)**

| Input | Action |
|-------|--------|
| Left Stick | Move |
| Right Stick | Virtual aim cursor |
| RT / LT | Fire / Aim mode |
| LS / RS | Sprint / Walk |
| X (□) | Reload |
| D-Pad ↑ | Health ability |
| Y (△) | Drop weapon |
| A (✕) | Interact / Pick up |
| RB / LB | Next / Previous weapon |
| Start | Menu |

---

### Third-Party Assets

| Asset | Link |
|-------|------|
| ProIconPack | [fab.com](https://fab.com/s/2b0926adaf34) |
| Military Weapons Dark — Adia Entertainment | [fab.com](https://www.fab.com/plugins/egl/sellers/Adia%20Entertainment) |
| MedievalDungeon | [fab.com](https://fab.com/s/494a7622e1be) |
| Gothic Knight | [fab.com](https://fab.com/s/2b8002297582) |
| Megascans — Blood | [Quixel / Epic](https://www.unrealengine.com/marketplace/en-US/product/e2368ad39c084e77a626dd424b1efcbe) |

---

MIT © Vl-Wolf

---

## Русский

# Descent

![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-4.27.2-black?logo=unrealengine)
![C++](https://img.shields.io/badge/C%2B%2B-70%25-blue?logo=cplusplus)
![Blueprint](https://img.shields.io/badge/Blueprint-AI%20%7C%20UI%20%7C%20Game-purple?logo=unrealengine)
![Multiplayer](https://img.shields.io/badge/Multiplayer-Co--op%20Listen%20Server-green)
![Platform](https://img.shields.io/badge/Platform-PC%20%7C%20Steam%20Deck-lightblue)
![License](https://img.shields.io/badge/License-MIT-lightgrey)

**Кооперативный шутер с видом сверху на C++ и Blueprints в Unreal Engine 4.27.2.**

Четыре класса оружия. Враги с AI и босс. Реплицированная система статус-эффектов. Полноценное лобби с кастомизацией. Играйте в одиночку или с другом — на PC или Steam Deck.

👾 **[Демо на itch.io →](https://vl-wolf.itch.io/descent)**

---

### Особенности

**Боевая система**
- 4 типа оружия — Винтовка, Дробовик, Снайперская винтовка, Гранатомёт
- **Разброс по состоянию движения** для каждого оружия (Aim / Walk / Run / Sprint — независимые кривые)
- Таймер гранаты с настраиваемым затуханием урона по радиусу
- Физически симулируемые гильзы и магазины при перезарядке
- Декали попаданий и частицы в зависимости от типа поверхности

**Враги и босс**
- Обычные враги с полным Behavior Tree: патруль → слух → преследование → атака
- **Босс** с отдельным BT, ракетными турелями (`BP_TurretRocketBoss`) и автономными `BP_RocketTurret`
- Волновой спавнер врагов (`BP_EnemySpawn` + `DT_Levels`)

**Статус-эффекты**
- Реплицированный Blueprintable `UTDS_StateEffect` трёх уровней: мгновенный, DoT, AoE
- Blueprint-реализации: огонь DoT, лечение (разово и со временем), площадной
- Фильтрация по поверхности, привязка к кости, стекируемость

**Инвентарь и пикапы**
- 4 слота оружия с отслеживанием патронов по типу
- Предметы в мире: оружие, патроны, аптечка
- Выброшенное оружие — физический пропс с импульсом

**Мультиплеер и лобби**
- Listen-сервер, всё состояние реплицировано и авторитетно на сервере
- Лобби: создание сессии, готовность игроков, **выбор цвета плаща** (4 цвета)
- HUD: здоровье, слоты оружия, патроны, плавающий урон, счёт убийств

**Окружение**
- Средневековый данж: интерактивные двери, камерные решётки, динамическая система факелов
- Иконки подсказок ввода автоматически меняются под клавиатуру или Xbox-геймпад
- Меню настроек с выбором уровня и браузером серверов

**Платформы**
- Windows (Win64) и **Steam Deck / Linux**
- Полная поддержка **Xbox-совместимого геймпада** (зона нечувствительности 0.25)

---

### Архитектура

```
C++ ядро                          Blueprint-слой
──────────────────────────────    ──────────────────────────────────────────
ATDSCharacter                     BP_Character
  ├ UTDSInventoryComponent           BP_WeaponDefault · Projectile · Grenade
  ├ UTDSCharacterHealthComponent     BP_CharacterEnemy_Base
  └ UTDS_EffectComponent                BT_AI_Base (патруль→слух→преследование→атака)
AWeaponDefault                     BP_CharacterEnemy_Boss
  └ AProjectileDefault                  BT_AI_Boss · BP_TurretRocketBoss
      └ AProjectileDefault_Grenade  BP_EnemySpawn
UTDS_StateEffect                   BP_StateEffect_Fire / Healing / AoE
  ├ ExecuteOnce                     BP_PickUpActor_Weapon / _Ammo / _FirstAid
  ├ ExecuteTimer                    BP_DoorSystem · BP_TorchSystem*
  └ AreaOfEffect                    WBP_HUD · BPW_InGameWidget · BPW_Menu
ATDSPlayerController               BP_LobbyMode · BP_LobbyPlayerController
UTDSGameInstance                   DT_WeaponInfo · DT_DropItemInfo · DT_Levels
```

---

### Технологии

| | |
|---|---|
| Движок | Unreal Engine 4.27.2 |
| Язык | C++ + Blueprints |
| AI | Behavior Tree + Blackboard |
| Сеть | Listen Server · Unreal Replication |
| Платформы | Windows (Win64) · Steam Deck / Linux |

---

### Управление

**Клавиатура и мышь**

| Ввод | Действие |
|------|----------|
| `W A S D` | Движение |
| `Мышь` | Прицел |
| `ЛКМ / ПКМ` | Стрельба / Прицеливание |
| `Left Shift / Left Alt` | Спринт / Ходьба |
| `R` | Перезарядка |
| `Z` | Способность (здоровье) |
| `1 – 4` | Слот оружия |
| `C / X` | Следующее / предыдущее |
| `G` | Выбросить оружие |
| `E` | Взаимодействие / Подбор |
| `Esc` | Меню |

**Геймпад (Xbox, зона нечувствительности 0.25)**

| Ввод | Действие |
|------|----------|
| Левый стик | Движение |
| Правый стик | Виртуальный прицел |
| RT / LT | Стрельба / Прицеливание |
| LS / RS | Спринт / Ходьба |
| X (□) | Перезарядка |
| D-Pad ↑ | Способность (здоровье) |
| Y (△) | Выбросить оружие |
| A (✕) | Взаимодействие / Подбор |
| RB / LB | Следующее / предыдущее |
| Start | Меню |

---

### Сторонние ассеты

| Ассет | Ссылка |
|-------|--------|
| ProIconPack | [fab.com](https://fab.com/s/2b0926adaf34) |
| Military Weapons Dark — Adia Entertainment | [fab.com](https://www.fab.com/plugins/egl/sellers/Adia%20Entertainment) |
| MedievalDungeon | [fab.com](https://fab.com/s/494a7622e1be) |
| Gothic Knight | [fab.com](https://fab.com/s/2b8002297582) |
| Megascans — Blood | [Quixel / Epic](https://www.unrealengine.com/marketplace/en-US/product/e2368ad39c084e77a626dd424b1efcbe) |

---

MIT © Vl-Wolf
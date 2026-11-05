#include <game/graphics/EffectInfo.h>
#include <revolution/types.h>

namespace {
__declspec(align(4)) char sEffectName_item_bakuhatu_star[] = "item_bakuhatu_star";
__declspec(align(4)) char sEffectName_item_mizu[] = "item_mizu";
__declspec(align(4)) char sEffectName_item_honoo[] = "item_honoo";
__declspec(align(4)) char sEffectName_item_bakuhatu_yellow[] = "item_bakuhatu_yellow";
__declspec(align(4)) char sEffectName_item_action[] = "item_action";
__declspec(align(4)) char sEffectName_item_nioidama[] = "item_nioidama";
__declspec(align(4)) char sEffectName_item_tatumaki[] = "item_tatumaki";
__declspec(align(4)) char sEffectName_item_pinwheel[] = "item_pinwheel";
__declspec(align(4)) char sEffectName_item_gurasidea[] = "item_gurasidea";
__declspec(align(4)) char sEffectName_Pii_kemuri[] = "Pii_kemuri";
__declspec(align(4)) char sEffectName_Pii_level100[] = "Pii_level100";
__declspec(align(4)) char sEffectName_Pii_haifu[] = "Pii_haifu";
__declspec(align(4)) char sEffectName_Pii_tyakuti[] = "Pii_tyakuti";
__declspec(align(4)) char sEffectName_Pii_attack_onpu[] = "Pii_attack_onpu";
__declspec(align(4)) char sEffectName_Pii_attack_normal[] = "Pii_attack_normal";
__declspec(align(4)) char sEffectName_Pii_receive_normal[] = "Pii_receive_normal";
__declspec(align(4)) char sEffectName_Pii_attack_honoo[] = "Pii_attack_honoo";
__declspec(align(4)) char sEffectName_Pii_receive_honoo[] = "Pii_receive_honoo";
__declspec(align(4)) char sEffectName_Pii_attack_mizu[] = "Pii_attack_mizu";
__declspec(align(4)) char sEffectName_Pii_receive_mizu[] = "Pii_receive_mizu";
__declspec(align(4)) char sEffectName_Pii_attack_denki[] = "Pii_attack_denki";
__declspec(align(4)) char sEffectName_Pii_receive_denki[] = "Pii_receive_denki";
__declspec(align(4)) char sEffectName_Pii_attack_kusa[] = "Pii_attack_kusa";
__declspec(align(4)) char sEffectName_Pii_receive_kusa[] = "Pii_receive_kusa";
__declspec(align(4)) char sEffectName_Pii_attack_esper[] = "Pii_attack_esper";
__declspec(align(4)) char sEffectName_Pii_receive_esper[] = "Pii_receive_esper";
__declspec(align(4)) char sEffectName_Pii_attack_kakutou[] = "Pii_attack_kakutou";
__declspec(align(4)) char sEffectName_Pii_receive_kakutou[] = "Pii_receive_kakutou";
__declspec(align(4)) char sEffectName_Pii_attack_doku[] = "Pii_attack_doku";
__declspec(align(4)) char sEffectName_Pii_receive_doku[] = "Pii_receive_doku";
__declspec(align(4)) char sEffectName_Pii_attack_jimen[] = "Pii_attack_jimen";
__declspec(align(4)) char sEffectName_Pii_attack_dragon[] = "Pii_attack_dragon";
__declspec(align(4)) char sEffectName_Pii_receive_dragon[] = "Pii_receive_dragon";
__declspec(align(4)) char sEffectName_Pii_attack_musi[] = "Pii_attack_musi";
__declspec(align(4)) char sEffectName_Pii_receive_musi[] = "Pii_receive_musi";
__declspec(align(4)) char sEffectName_Pii_attack_iwa[] = "Pii_attack_iwa";
__declspec(align(4)) char sEffectName_Pii_attack_ghost[] = "Pii_attack_ghost";
__declspec(align(4)) char sEffectName_Pii_attack_koori[] = "Pii_attack_koori";
__declspec(align(4)) char sEffectName_Pii_attack_hagane[] = "Pii_attack_hagane";
__declspec(align(4)) char sEffectName_Pii_receive_hagane[] = "Pii_receive_hagane";
__declspec(align(4)) char sEffectName_Pii_attack_aku[] = "Pii_attack_aku";
__declspec(align(4)) char sEffectName_Pii_receive_aku[] = "Pii_receive_aku";
__declspec(align(4)) char sEffectName_stage_parade[] = "stage_parade";
__declspec(align(4)) char sEffectName_Mii_warp[] = "Mii_warp";
__declspec(align(4)) char sEffectName_Mii_kirakira[] = "Mii_kirakira";
__declspec(align(4)) char sEffectName_Mii_feed[] = "Mii_feed";
__declspec(align(4)) char sEffectName_Pii_flash[] = "Pii_flash";
__declspec(align(4)) char sEffectName_Ball_star[] = "Ball_star";
__declspec(align(4)) char sEffectName_Ball_kiran[] = "Ball_kiran";
__declspec(align(4)) char sEffectName_Pii_syokai[] = "Pii_syokai";
__declspec(align(4)) char sEffectName_Pii_suisui_jump[] = "Pii_suisui_jump";
__declspec(align(4)) char sEffectName_stage_rain[] = "stage_rain";
__declspec(align(4)) char sEffectName_stage_snow[] = "stage_snow";
__declspec(align(4)) char sEffectName_event_neji_star[] = "event_neji_star";
__declspec(align(4)) char sEffectName_event_neji_light[] = "event_neji_light";
__declspec(align(4)) char sEffectName_event_neji_get_whiteout[] = "event_neji_get_whiteout";
__declspec(align(4)) char sEffectName_EFFECT_neji[] = "EFFECT_neji";
__declspec(align(4)) char sEffectName_event_seisan[] = "event_seisan";
__declspec(align(4)) char sEffectName_event_kamifubuki[] = "event_kamifubuki";
__declspec(align(4)) char sEffectName_EFFECT_storm[] = "EFFECT_storm";
__declspec(align(4)) char sEffectName_event_boss_neji[] = "event_boss_neji";
__declspec(align(4)) char sEffectName_event_boss[] = "event_boss";
__declspec(align(4)) char sEffectName_stage_rope[] = "stage_rope";
__declspec(align(4)) char sEffectName_hit_critical[] = "hit_critical";
__declspec(align(4)) char sEffectName_hit_saisei[] = "hit_saisei";
__declspec(align(4)) char sEffectName_hit_bomb[] = "hit_bomb";
__declspec(align(4)) char sEffectName_hit_bomb_s[] = "hit_bomb_s";
__declspec(align(4)) char sEffectName_born_boss[] = "born_boss";
__declspec(align(4)) char sEffectName_die_normal[] = "die_normal";
__declspec(align(4)) char sEffectName_die_capsule[] = "die_capsule";
__declspec(align(4)) char sEffectName_item_get[] = "item_get";
__declspec(align(4)) char sEffectName_item_capsule[] = "item_capsule";
__declspec(align(4)) char sEffectName_otamesi_out[] = "otamesi_out";
__declspec(align(4)) char sEffectName_otamesi_in[] = "otamesi_in";
__declspec(align(4)) char sEffectName_drop_boss[] = "drop_boss";
__declspec(align(4)) char sEffectName_aura_rival[] = "aura_rival";
__declspec(align(4)) char sEffectName_aura_MYUUTUU[] = "aura_MYUUTUU";
__declspec(align(4)) char sEffectName_item_capsule_01[] = "item_capsule_01";
__declspec(align(4)) char sEffectName_item_capsule_02[] = "item_capsule_02";
__declspec(align(4)) char sEffectName_item_capsule_03[] = "item_capsule_03";
__declspec(align(4)) char sEffectName_item_capsule_04[] = "item_capsule_04";
__declspec(align(4)) char sEffectName_item_capsule_05[] = "item_capsule_05";
__declspec(align(4)) char sEffectName_walk_fire[] = "walk_fire";
__declspec(align(4)) char sEffectName_walk_grass[] = "walk_grass";
__declspec(align(4)) char sEffectName_walk_sand[] = "walk_sand";
__declspec(align(4)) char sEffectName_walk_soil[] = "walk_soil";
__declspec(align(4)) char sEffectName_walk_water[] = "walk_water";
__declspec(align(4)) char sEffectName_ijo_poison[] = "ijo_poison";
__declspec(align(4)) char sEffectName_ijo_dokudoku[] = "ijo_dokudoku";
__declspec(align(4)) char sEffectName_ijo_yakedo[] = "ijo_yakedo";
__declspec(align(4)) char sEffectName_ijo_mahi[] = "ijo_mahi";
__declspec(align(4)) char sEffectName_ijo_koori[] = "ijo_koori";
__declspec(align(4)) char sEffectName_ijo_nemuri[] = "ijo_nemuri";
__declspec(align(4)) char sEffectName_ijo_konran[] = "ijo_konran";
__declspec(align(4)) char sEffectName_ijo_hirumu[] = "ijo_hirumu";
__declspec(align(4)) char sEffectName_ijo_down[] = "ijo_down";
__declspec(align(4)) char sEffectName_ijo_kaifuku[] = "ijo_kaifuku";
__declspec(align(4)) char sEffectName_ijo_kizetu[] = "ijo_kizetu";
__declspec(align(4)) char sEffectName_ijo_NEWOHARU[] = "ijo_NEWOHARU";
__declspec(align(4)) char sEffectName_ijo_KOKORONOME[] = "ijo_KOKORONOME";
__declspec(align(4)) char sEffectName_ijo_ROKKUON[] = "ijo_ROKKUON";
__declspec(align(4)) char sEffectName_ijo_MARUKUNARU[] = "ijo_MARUKUNARU";
__declspec(align(4)) char sEffectName_kaifuku_short[] = "kaifuku_short";
__declspec(align(4)) char sEffectName_tame_boss_01[] = "tame_boss_01";
__declspec(align(4)) char sEffectName_tame_boss_02[] = "tame_boss_02";
__declspec(align(4)) char sEffectName_tame_boss_03[] = "tame_boss_03";
__declspec(align(4)) char sEffectName_attack_iwa[] = "attack_iwa";
__declspec(align(4)) char sEffectName_attack_honoo[] = "attack_honoo";
__declspec(align(4)) char sEffectName_atk_electric_Jet[] = "atk_electric_Jet";
__declspec(align(4)) char sEffectName_atk_electric_Fall[] = "atk_electric_Fall";
__declspec(align(4)) char sEffectName_atk_fire_Bomb[] = "atk_fire_Bomb";
__declspec(align(4)) char sEffectName_atk_fire_Jet[] = "atk_fire_Jet";
__declspec(align(4)) char sEffectName_atk_fire_Jet_2[] = "atk_fire_Jet_2";
__declspec(align(4)) char sEffectName_atk_fire_Jet_3[] = "atk_fire_Jet_3";
__declspec(align(4)) char sEffectName_atk_fire_Shoot[] = "atk_fire_Shoot";
__declspec(align(4)) char sEffectName_atk_fire_Shoot_wide[] = "atk_fire_Shoot_wide";
__declspec(align(4)) char sEffectName_atk_iwa_Jet[] = "atk_iwa_Jet";
__declspec(align(4)) char sEffectName_atk_iwa_Fall[] = "atk_iwa_Fall";
__declspec(align(4)) char sEffectName_atk_kusa_Jet[] = "atk_kusa_Jet";
__declspec(align(4)) char sEffectName_atk_normal_Jet_onpu[] = "atk_normal_Jet_onpu";
__declspec(align(4)) char sEffectName_atk_poison_Jet[] = "atk_poison_Jet";
__declspec(align(4)) char sEffectName_atk_sp_Jet[] = "atk_sp_Jet";
__declspec(align(4)) char sEffectName_atk_water_Jet[] = "atk_water_Jet";
__declspec(align(4)) char sEffectName_atk_mushi_Jet[] = "atk_mushi_Jet";
__declspec(align(4)) char sEffectName_atk_range[] = "atk_range";
__declspec(align(4)) char sEffectName_atk_battle_Adjacent[] = "atk_battle_Adjacent";
__declspec(align(4)) char sEffectName_atk_fire_Adjacent[] = "atk_fire_Adjacent";
__declspec(align(4)) char sEffectName_atk_poison_Adjacent[] = "atk_poison_Adjacent";
__declspec(align(4)) char sEffectName_atk_fire_Round[] = "atk_fire_Round";
__declspec(align(4)) char sEffectName_atk_kusa_Round[] = "atk_kusa_Round";
__declspec(align(4)) char sEffectName_atk_poison_Round[] = "atk_poison_Round";
__declspec(align(4)) char sEffectName_atk_fire_Laser[] = "atk_fire_Laser";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_HIKKAKU[] = "atk_normal_kinsetu_HIKKAKU";
__declspec(align(4)) char sEffectName_atk_normal_cube_02[] = "atk_normal_cube_02";
__declspec(align(4)) char sEffectName_atk_normal_ougi_01[] = "atk_normal_ougi_01";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_ABARERU[] = "atk_normal_tyuusin_ABARERU";
__declspec(align(4)) char sEffectName_atk_normal_ido_SINSOKU[] = "atk_normal_ido_SINSOKU";
__declspec(align(4)) char sEffectName_atk_normal_shoot_02[] = "atk_normal_shoot_02";
__declspec(align(4)) char sEffectName_atk_hikou_enkaku_01[] = "atk_hikou_enkaku_01";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_MAHHAPANTI[] = "atk_battle_kinsetu_MAHHAPANTI";
__declspec(align(4)) char sEffectName_atk_battle_tyuusin_01[] = "atk_battle_tyuusin_01";
__declspec(align(4)) char sEffectName_atk_battle_ido_01[] = "atk_battle_ido_01";
__declspec(align(4)) char sEffectName_atk_battle_shoot_01[] = "atk_battle_shoot_01";
__declspec(align(4)) char sEffectName_atk_hikou_kinsetu_01[] = "atk_hikou_kinsetu_01";
__declspec(align(4)) char sEffectName_atk_hikou_cube_01[] = "atk_hikou_cube_01";
__declspec(align(4)) char sEffectName_atk_hikou_ido_01[] = "atk_hikou_ido_01";
__declspec(align(4)) char sEffectName_atk_poison_kinsetu_01[] = "atk_poison_kinsetu_01";
__declspec(align(4)) char sEffectName_atk_poison_cube_01[] = "atk_poison_cube_01";
__declspec(align(4)) char sEffectName_atk_poison_ougi_01[] = "atk_poison_ougi_01";
__declspec(align(4)) char sEffectName_atk_poison_shoot_01[] = "atk_poison_shoot_01";
__declspec(align(4)) char sEffectName_atk_poison_enkaku_01[] = "atk_poison_enkaku_01";
__declspec(align(4)) char sEffectName_atk_jimen_kinsetu_01[] = "atk_jimen_kinsetu_01";
__declspec(align(4)) char sEffectName_atk_jimen_cube_01[] = "atk_jimen_cube_01";
__declspec(align(4)) char sEffectName_atk_jimen_tyuusin_01[] = "atk_jimen_tyuusin_01";
__declspec(align(4)) char sEffectName_atk_jimen_ido_01[] = "atk_jimen_ido_01";
__declspec(align(4)) char sEffectName_atk_jimen_shoot_01[] = "atk_jimen_shoot_01";
__declspec(align(4)) char sEffectName_atk_jimen_enkaku_01[] = "atk_jimen_enkaku_01";
__declspec(align(4)) char sEffectName_atk_iwa_kinsetu_01[] = "atk_iwa_kinsetu_01";
__declspec(align(4)) char sEffectName_atk_iwa_cube_01[] = "atk_iwa_cube_01";
__declspec(align(4)) char sEffectName_atk_iwa_ougi_01[] = "atk_iwa_ougi_01";
__declspec(align(4)) char sEffectName_atk_iwa_ido_01[] = "atk_iwa_ido_01";
__declspec(align(4)) char sEffectName_atk_iwa_shoot_01[] = "atk_iwa_shoot_01";
__declspec(align(4)) char sEffectName_atk_iwa_enkaku_01[] = "atk_iwa_enkaku_01";
__declspec(align(4)) char sEffectName_atk_mushi_kinsetu_01[] = "atk_mushi_kinsetu_01";
__declspec(align(4)) char sEffectName_atk_mushi_cube_01[] = "atk_mushi_cube_01";
__declspec(align(4)) char sEffectName_atk_mushi_ougi_02[] = "atk_mushi_ougi_02";
__declspec(align(4)) char sEffectName_atk_mushi_ido_01[] = "atk_mushi_ido_01";
__declspec(align(4)) char sEffectName_atk_mushi_shoot_01[] = "atk_mushi_shoot_01";
__declspec(align(4)) char sEffectName_atk_ghost_kinsetu_01[] = "atk_ghost_kinsetu_01";
__declspec(align(4)) char sEffectName_atk_ghost_cube_01[] = "atk_ghost_cube_01";
__declspec(align(4)) char sEffectName_atk_ghost_ido_01[] = "atk_ghost_ido_01";
__declspec(align(4)) char sEffectName_atk_ghost_shoot_01[] = "atk_ghost_shoot_01";
__declspec(align(4)) char sEffectName_atk_ghost_enkaku_01[] = "atk_ghost_enkaku_01";
__declspec(align(4)) char sEffectName_atk_metal_kinsetu_01[] = "atk_metal_kinsetu_01";
__declspec(align(4)) char sEffectName_atk_metal_cube_01[] = "atk_metal_cube_01";
__declspec(align(4)) char sEffectName_atk_metal_shoot_01[] = "atk_metal_shoot_01";
__declspec(align(4)) char sEffectName_atk_fire_kinsetu_01[] = "atk_fire_kinsetu_01";
__declspec(align(4)) char sEffectName_atk_fire_cube_01[] = "atk_fire_cube_01";
__declspec(align(4)) char sEffectName_atk_fire_ougi_SEINARUHONOO[] = "atk_fire_ougi_SEINARUHONOO";
__declspec(align(4)) char sEffectName_atk_fire_tyuusin_01[] = "atk_fire_tyuusin_01";
__declspec(align(4)) char sEffectName_atk_fire_ido_01[] = "atk_fire_ido_01";
__declspec(align(4)) char sEffectName_atk_fire_shoot[] = "atk_fire_shoot";
__declspec(align(4)) char sEffectName_atk_water_kinsetu_01[] = "atk_water_kinsetu_01";
__declspec(align(4)) char sEffectName_atk_water_cube_01[] = "atk_water_cube_01";
__declspec(align(4)) char sEffectName_atk_water_ougi_AWA[] = "atk_water_ougi_AWA";
__declspec(align(4)) char sEffectName_atk_water_ido_01[] = "atk_water_ido_01";
__declspec(align(4)) char sEffectName_atk_water_shoot_01[] = "atk_water_shoot_01";
__declspec(align(4)) char sEffectName_atk_water_enkaku_01[] = "atk_water_enkaku_01";
__declspec(align(4)) char sEffectName_atk_kusa_kinsetu_01[] = "atk_kusa_kinsetu_01";
__declspec(align(4)) char sEffectName_atk_kusa_cube_01[] = "atk_kusa_cube_01";
__declspec(align(4)) char sEffectName_atk_kusa_ougi_01[] = "atk_kusa_ougi_01";
__declspec(align(4)) char sEffectName_atk_kusa_tyuusin_01[] = "atk_kusa_tyuusin_01";
__declspec(align(4)) char sEffectName_atk_kusa_shoot_01[] = "atk_kusa_shoot_01";
__declspec(align(4)) char sEffectName_atk_kusa_enkaku_01[] = "atk_kusa_enkaku_01";
__declspec(align(4)) char sEffectName_atk_electric_kinsetu_01[] = "atk_electric_kinsetu_01";
__declspec(align(4)) char sEffectName_atk_electric_cube_01[] = "atk_electric_cube_01";
__declspec(align(4)) char sEffectName_atk_electric_ougi_01[] = "atk_electric_ougi_01";
__declspec(align(4)) char sEffectName_atk_electric_tyuusin_01[] = "atk_electric_tyuusin_01";
__declspec(align(4)) char sEffectName_atk_electric_ido_01[] = "atk_electric_ido_01";
__declspec(align(4)) char sEffectName_atk_electric_shoot_01[] = "atk_electric_shoot_01";
__declspec(align(4)) char sEffectName_atk_electric_enkaku_01[] = "atk_electric_enkaku_01";
__declspec(align(4)) char sEffectName_atk_sp_cube_01[] = "atk_sp_cube_01";
__declspec(align(4)) char sEffectName_atk_sp_ougi_01[] = "atk_sp_ougi_01";
__declspec(align(4)) char sEffectName_atk_sp_ido_01[] = "atk_sp_ido_01";
__declspec(align(4)) char sEffectName_atk_sp_shoot_01[] = "atk_sp_shoot_01";
__declspec(align(4)) char sEffectName_atk_sp_enkaku_01[] = "atk_sp_enkaku_01";
__declspec(align(4)) char sEffectName_atk_koori_kinsetu_01[] = "atk_koori_kinsetu_01";
__declspec(align(4)) char sEffectName_atk_koori_cube_01[] = "atk_koori_cube_01";
__declspec(align(4)) char sEffectName_atk_koori_ougi_02[] = "atk_koori_ougi_02";
__declspec(align(4)) char sEffectName_atk_koori_shoot_02[] = "atk_koori_shoot_02";
__declspec(align(4)) char sEffectName_atk_dragon_kinsetu_01[] = "atk_dragon_kinsetu_01";
__declspec(align(4)) char sEffectName_atk_dragon_cube_01[] = "atk_dragon_cube_01";
__declspec(align(4)) char sEffectName_atk_dragon_tyuusin_01[] = "atk_dragon_tyuusin_01";
__declspec(align(4)) char sEffectName_atk_dragon_ido_01[] = "atk_dragon_ido_01";
__declspec(align(4)) char sEffectName_atk_dragon_enkaku_01[] = "atk_dragon_enkaku_01";
__declspec(align(4)) char sEffectName_atk_aku_kinsetu_HUIUTI[] = "atk_aku_kinsetu_HUIUTI";
__declspec(align(4)) char sEffectName_atk_aku_cube_01[] = "atk_aku_cube_01";
__declspec(align(4)) char sEffectName_atk_aku_ido_01[] = "atk_aku_ido_01";
__declspec(align(4)) char sEffectName_atk_water_ido_02[] = "atk_water_ido_02";
__declspec(align(4)) char sEffectName_atk_normal_ido_ZUTUKI[] = "atk_normal_ido_ZUTUKI";
__declspec(align(4)) char sEffectName_atk_normal_ido_SUTEMITAKKURU[] = "atk_normal_ido_SUTEMITAKKURU";
__declspec(align(4)) char sEffectName_atk_normal_ido_TAIATARI[] = "atk_normal_ido_TAIATARI";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_TATAKITUKERU[] = "atk_normal_kinsetu_TATAKITUKERU";
__declspec(align(4)) char sEffectName_atk_normal_ido_TUNODETUKU[] = "atk_normal_ido_TUNODETUKU";
__declspec(align(4)) char sEffectName_atk_normal_ido_DENKOUSEKKA[] = "atk_normal_ido_DENKOUSEKKA";
__declspec(align(4)) char sEffectName_atk_normal_ido_TOSSIN[] = "atk_normal_ido_TOSSIN";
__declspec(align(4)) char sEffectName_atk_normal_ido_NOSIKAKARI[] = "atk_normal_ido_NOSIKAKARI";
__declspec(align(4)) char sEffectName_atk_normal_ido_MIDAREDUKI[] = "atk_normal_ido_MIDAREDUKI";
__declspec(align(4)) char sEffectName_atk_normal_ido_ROKETTOZUTUKI[] = "atk_normal_ido_ROKETTOZUTUKI";
__declspec(align(4)) char sEffectName_atk_normal_ido_ROKKUKURAIMU[] = "atk_normal_ido_ROKKUKURAIMU";
__declspec(align(4)) char sEffectName_atk_iwa_ido_02[] = "atk_iwa_ido_02";
__declspec(align(4)) char sEffectName_atk_hikou_ido_02[] = "atk_hikou_ido_02";
__declspec(align(4)) char sEffectName_atk_hikou_ido_03[] = "atk_hikou_ido_03";
__declspec(align(4)) char sEffectName_atk_hikou_ido_04[] = "atk_hikou_ido_04";
__declspec(align(4)) char sEffectName_atk_hikou_ido_05[] = "atk_hikou_ido_05";
__declspec(align(4)) char sEffectName_atk_hikou_ido_06[] = "atk_hikou_ido_06";
__declspec(align(4)) char sEffectName_atk_ghost_ido_02[] = "atk_ghost_ido_02";
__declspec(align(4)) char sEffectName_atk_battle_ido_02[] = "atk_battle_ido_02";
__declspec(align(4)) char sEffectName_atk_sp_shoot_02[] = "atk_sp_shoot_02";
__declspec(align(4)) char sEffectName_atk_poison_shoot_02[] = "atk_poison_shoot_02";
__declspec(align(4)) char sEffectName_atk_poison_shoot_03[] = "atk_poison_shoot_03";
__declspec(align(4)) char sEffectName_atk_poison_cube_HEDOROKOUGEKI[] = "atk_poison_cube_HEDOROKOUGEKI";
__declspec(align(4)) char sEffectName_atk_normal_shoot_01[] = "atk_normal_shoot_01";
__declspec(align(4)) char sEffectName_atk_normal_shoot_03[] = "atk_normal_shoot_03";
__declspec(align(4)) char sEffectName_atk_normal_shoot_04[] = "atk_normal_shoot_04";
__declspec(align(4)) char sEffectName_atk_normal_shoot_05[] = "atk_normal_shoot_05";
__declspec(align(4)) char sEffectName_atk_normal_shoot_06[] = "atk_normal_shoot_06";
__declspec(align(4)) char sEffectName_atk_mushi_shoot_02[] = "atk_mushi_shoot_02";
__declspec(align(4)) char sEffectName_atk_metal_shoot_02[] = "atk_metal_shoot_02";
__declspec(align(4)) char sEffectName_atk_kusa_shoot_02[] = "atk_kusa_shoot_02";
__declspec(align(4)) char sEffectName_atk_kusa_shoot_03[] = "atk_kusa_shoot_03";
__declspec(align(4)) char sEffectName_atk_kusa_shoot_04[] = "atk_kusa_shoot_04";
__declspec(align(4)) char sEffectName_atk_koori_shoot_03[] = "atk_koori_shoot_03";
__declspec(align(4)) char sEffectName_atk_iwa_shoot_02[] = "atk_iwa_shoot_02";
__declspec(align(4)) char sEffectName_atk_battle_shoot_02[] = "atk_battle_shoot_02";
__declspec(align(4)) char sEffectName_atk_kusa_jibun_01[] = "atk_kusa_jibun_01";
__declspec(align(4)) char sEffectName_atk_battle_tyuusin_02[] = "atk_battle_tyuusin_02";
__declspec(align(4)) char sEffectName_atk_kusa_tyuusin_02[] = "atk_kusa_tyuusin_02";
__declspec(align(4)) char sEffectName_atk_jimen_tyuusin_02[] = "atk_jimen_tyuusin_02";
__declspec(align(4)) char sEffectName_atk_electric_tyuusin_02[] = "atk_electric_tyuusin_02";
__declspec(align(4)) char sEffectName_atk_dragon_tyuusin_02[] = "atk_dragon_tyuusin_02";
__declspec(align(4)) char sEffectName_atk_fire_tyuusin_02[] = "atk_fire_tyuusin_02";
__declspec(align(4)) char sEffectName_atk_fire_tyuusin_03[] = "atk_fire_tyuusin_03";
__declspec(align(4)) char sEffectName_atk_aku_kinsetu_TUZIGIRI[] = "atk_aku_kinsetu_TUZIGIRI";
__declspec(align(4)) char sEffectName_atk_aku_kinsetu_OIUTI[] = "atk_aku_kinsetu_OIUTI";
__declspec(align(4)) char sEffectName_atk_aku_kinsetu_OSIOKI[] = "atk_aku_kinsetu_OSIOKI";
__declspec(align(4)) char sEffectName_atk_aku_kinsetu_KAMITUKU[] = "atk_aku_kinsetu_KAMITUKU";
__declspec(align(4)) char sEffectName_atk_aku_kinsetu_KAMIKUDAKU[] = "atk_aku_kinsetu_KAMIKUDAKU";
__declspec(align(4)) char sEffectName_atk_ghost_kinsetu_02[] = "atk_ghost_kinsetu_02";
__declspec(align(4)) char sEffectName_atk_ghost_kinsetu_03[] = "atk_ghost_kinsetu_03";
__declspec(align(4)) char sEffectName_atk_koori_kinsetu_02[] = "atk_koori_kinsetu_02";
__declspec(align(4)) char sEffectName_atk_jimen_kinsetu_02[] = "atk_jimen_kinsetu_02";
__declspec(align(4)) char sEffectName_atk_electric_kinsetu_02[] = "atk_electric_kinsetu_02";
__declspec(align(4)) char sEffectName_atk_electric_kinsetu_03[] = "atk_electric_kinsetu_03";
__declspec(align(4)) char sEffectName_atk_poison_kinsetu_02[] = "atk_poison_kinsetu_02";
__declspec(align(4)) char sEffectName_atk_poison_kinsetu_03[] = "atk_poison_kinsetu_03";
__declspec(align(4)) char sEffectName_atk_poison_kinsetu_04[] = "atk_poison_kinsetu_04";
__declspec(align(4)) char sEffectName_atk_poison_kinsetu_05[] = "atk_poison_kinsetu_05";
__declspec(align(4)) char sEffectName_atk_metal_kinsetu_02[] = "atk_metal_kinsetu_02";
__declspec(align(4)) char sEffectName_atk_metal_kinsetu_03[] = "atk_metal_kinsetu_03";
__declspec(align(4)) char sEffectName_atk_metal_cube_BARETTOPANTI[] = "atk_metal_cube_BARETTOPANTI";
__declspec(align(4)) char sEffectName_atk_metal_kinsetu_05[] = "atk_metal_kinsetu_05";
__declspec(align(4)) char sEffectName_atk_hikou_kinsetu_02[] = "atk_hikou_kinsetu_02";
__declspec(align(4)) char sEffectName_atk_hikou_kinsetu_03[] = "atk_hikou_kinsetu_03";
__declspec(align(4)) char sEffectName_atk_fire_kinsetu_02[] = "atk_fire_kinsetu_02";
__declspec(align(4)) char sEffectName_atk_fire_kinsetu_03[] = "atk_fire_kinsetu_03";
__declspec(align(4)) char sEffectName_atk_fire_kinsetu_04[] = "atk_fire_kinsetu_04";
__declspec(align(4)) char sEffectName_atk_water_ougi_KARADEHASAMU[] = "atk_water_ougi_KARADEHASAMU";
__declspec(align(4)) char sEffectName_atk_water_kinsetu_03[] = "atk_water_kinsetu_03";
__declspec(align(4)) char sEffectName_atk_mushi_kinsetu_02[] = "atk_mushi_kinsetu_02";
__declspec(align(4)) char sEffectName_atk_mushi_kinsetu_03[] = "atk_mushi_kinsetu_03";
__declspec(align(4)) char sEffectName_atk_iwa_enkaku_02[] = "atk_iwa_enkaku_02";
__declspec(align(4)) char sEffectName_atk_iwa_enkaku_03[] = "atk_iwa_enkaku_03";
__declspec(align(4)) char sEffectName_atk_sp_enkaku_02[] = "atk_sp_enkaku_02";
__declspec(align(4)) char sEffectName_atk_sp_enkaku_03[] = "atk_sp_enkaku_03";
__declspec(align(4)) char sEffectName_atk_jimen_enkaku_02[] = "atk_jimen_enkaku_02";
__declspec(align(4)) char sEffectName_atk_electric_enkaku_02[] = "atk_electric_enkaku_02";
__declspec(align(4)) char sEffectName_atk_dragon_enkaku_02[] = "atk_dragon_enkaku_02";
__declspec(align(4)) char sEffectName_atk_sp_ougi_02[] = "atk_sp_ougi_02";
__declspec(align(4)) char sEffectName_atk_sp_cube_02[] = "atk_sp_cube_02";
__declspec(align(4)) char sEffectName_atk_sp_cube_03[] = "atk_sp_cube_03";
__declspec(align(4)) char sEffectName_atk_kusa_ougi_02[] = "atk_kusa_ougi_02";
__declspec(align(4)) char sEffectName_atk_kusa_ougi_03[] = "atk_kusa_ougi_03";
__declspec(align(4)) char sEffectName_atk_kusa_ougi_04[] = "atk_kusa_ougi_04";
__declspec(align(4)) char sEffectName_atk_ghost_cube_AYASIIKAZE[] = "atk_ghost_cube_AYASIIKAZE";
__declspec(align(4)) char sEffectName_atk_koori_cube_KOGOERUKAZE[] = "atk_koori_cube_KOGOERUKAZE";
__declspec(align(4)) char sEffectName_atk_koori_cube_HUBUKI[] = "atk_koori_cube_HUBUKI";
__declspec(align(4)) char sEffectName_atk_koori_cube_02[] = "atk_koori_cube_02";
__declspec(align(4)) char sEffectName_atk_poison_cube_02[] = "atk_poison_cube_02";
__declspec(align(4)) char sEffectName_atk_dragon_cube_02[] = "atk_dragon_cube_02";
__declspec(align(4)) char sEffectName_atk_dragon_cube_03[] = "atk_dragon_cube_03";
__declspec(align(4)) char sEffectName_atk_dragon_cube_04[] = "atk_dragon_cube_04";
__declspec(align(4)) char sEffectName_atk_normal_cube_01[] = "atk_normal_cube_01";
__declspec(align(4)) char sEffectName_atk_normal_cube_03[] = "atk_normal_cube_03";
__declspec(align(4)) char sEffectName_atk_hikou_cube_EAKATTAA[] = "atk_hikou_cube_EAKATTAA";
__declspec(align(4)) char sEffectName_atk_water_cube_02[] = "atk_water_cube_02";
__declspec(align(4)) char sEffectName_atk_water_cube_03[] = "atk_water_cube_03";
__declspec(align(4)) char sEffectName_atk_water_cube_04[] = "atk_water_cube_04";
__declspec(align(4)) char sEffectName_atk_water_cube_05[] = "atk_water_cube_05";
__declspec(align(4)) char sEffectName_atk_mushi_cube_GINIRONOKAZE[] = "atk_mushi_cube_GINIRONOKAZE";
__declspec(align(4)) char sEffectName_atk_electric_kinsetu_KAMINARIPANTI[] = "atk_electric_kinsetu_KAMINARIPANTI";
__declspec(align(4)) char sEffectName_atk_fire_kinsetu_ONIBI[] = "atk_fire_kinsetu_ONIBI";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_AKUBI[] = "atk_normal_tyuusin_AKUBI";
__declspec(align(4)) char sEffectName_atk_normal_ougi_AKUBI[] = "atk_normal_ougi_AKUBI";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_UTAU[] = "atk_normal_tyuusin_UTAU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_HURAHURADANSU[] = "atk_normal_tyuusin_HURAHURADANSU";
__declspec(align(4)) char sEffectName_atk_kusa_ougi_KINOKONOHOUSI[] = "atk_kusa_ougi_KINOKONOHOUSI";
__declspec(align(4)) char sEffectName_atk_ghost_ougi_AYASIIHIKARI[] = "atk_ghost_ougi_AYASIIHIKARI";
__declspec(align(4)) char sEffectName_atk_kusa_ougi_NEMURIGONA[] = "atk_kusa_ougi_NEMURIGONA";
__declspec(align(4)) char sEffectName_atk_normal_ougi_IBARU[] = "atk_normal_ougi_IBARU";
__declspec(align(4)) char sEffectName_atk_aku_enkaku_ODATERU[] = "atk_aku_enkaku_ODATERU";
__declspec(align(4)) char sEffectName_atk_normal_cube_TYOUONPA[] = "atk_normal_cube_TYOUONPA";
__declspec(align(4)) char sEffectName_atk_fire_ougi_DAIMONZI[] = "atk_fire_ougi_DAIMONZI";
__declspec(align(4)) char sEffectName_atk_fire_cube_NEPPUU[] = "atk_fire_cube_NEPPUU";
__declspec(align(4)) char sEffectName_atk_fire_enkaku_HUNKA[] = "atk_fire_enkaku_HUNKA";
__declspec(align(4)) char sEffectName_atk_water_enkaku_SIOHUKI[] = "atk_water_enkaku_SIOHUKI";
__declspec(align(4)) char sEffectName_atk_water_cube_DAKURYUU[] = "atk_water_cube_DAKURYUU";
__declspec(align(4)) char sEffectName_atk_water_ougi_MIZUNOHADOU[] = "atk_water_ougi_MIZUNOHADOU";
__declspec(align(4)) char sEffectName_atk_water_enkaku_UZUSIO[] = "atk_water_enkaku_UZUSIO";
__declspec(align(4)) char sEffectName_atk_fire_enkaku_HONOONOUZU[] = "atk_fire_enkaku_HONOONOUZU";
__declspec(align(4)) char sEffectName_atk_fire_enkaku_MAGUMASUTOOMU[] = "atk_fire_enkaku_MAGUMASUTOOMU";
__declspec(align(4)) char sEffectName_atk_fire_cube_BURASUTOBAAN[] = "atk_fire_cube_BURASUTOBAAN";
__declspec(align(4)) char sEffectName_atk_water_cube_NAMINORI[] = "atk_water_cube_NAMINORI";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_KAMAITATI[] = "atk_normal_tyuusin_KAMAITATI";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_KOUSOKUSUPIN[] = "atk_normal_tyuusin_KOUSOKUSUPIN";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_SAWAGU[] = "atk_normal_tyuusin_SAWAGU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_ZITABATA[] = "atk_normal_tyuusin_ZITABATA";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_ZIBAKU[] = "atk_normal_tyuusin_ZIBAKU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_DAIBAKUHATU[] = "atk_normal_tyuusin_DAIBAKUHATU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_MEZAMERUPAWAA[] = "atk_normal_tyuusin_MEZAMERUPAWAA";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_MAWASIGERI[] = "atk_battle_kinsetu_MAWASIGERI";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_AAMUHANMAA[] = "atk_battle_kinsetu_AAMUHANMAA";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_ATEMINAGE[] = "atk_battle_kinsetu_ATEMINAGE";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_IWAKUDAKI[] = "atk_battle_kinsetu_IWAKUDAKI";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_INFAITO[] = "atk_battle_kinsetu_INFAITO";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_KARATETYOPPU[] = "atk_battle_kinsetu_KARATETYOPPU";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_KAWARAWARI[] = "atk_battle_kinsetu_KAWARAWARI";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_KUROSUTYOPPU[] = "atk_battle_kinsetu_KUROSUTYOPPU";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_KETAGURI[] = "atk_battle_kinsetu_KETAGURI";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_SINKUUHA[] = "atk_battle_kinsetu_SINKUUHA";
__declspec(align(4)) char sEffectName_atk_battle_ido_SUKAIAPPAA[] = "atk_battle_ido_SUKAIAPPAA";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_TIKYUUNAGE[] = "atk_battle_kinsetu_TIKYUUNAGE";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_TUPPARI[] = "atk_battle_kinsetu_TUPPARI";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_TORIPURUKIKKU[] = "atk_battle_kinsetu_TORIPURUKIKKU";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_DOREINPANTI[] = "atk_battle_kinsetu_DOREINPANTI";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_NIDOGERI[] = "atk_battle_kinsetu_NIDOGERI";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_BAKADIKARA[] = "atk_battle_kinsetu_BAKADIKARA";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_BAKURETUPANTI[] = "atk_battle_kinsetu_BAKURETUPANTI";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_HAKKEI[] = "atk_battle_kinsetu_HAKKEI";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_MEZAMASIBINTA[] = "atk_battle_kinsetu_MEZAMASIBINTA";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_IAIGIRI[] = "atk_normal_kinsetu_IAIGIRI";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_IKARI[] = "atk_normal_kinsetu_IKARI";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_IKARINOMAEBA[] = "atk_normal_kinsetu_IKARINOMAEBA";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_OUHUKUBINTA[] = "atk_normal_kinsetu_OUHUKUBINTA";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_KAIRIKI[] = "atk_normal_kinsetu_KAIRIKI";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_KARAGENKI[] = "atk_normal_kinsetu_KARAGENKI";
__declspec(align(4)) char sEffectName_atk_normal_ougi_KARAMITUKU[] = "atk_normal_ougi_KARAMITUKU";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_GIGAINPAKUTO[] = "atk_normal_kinsetu_GIGAINPAKUTO";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_KITUKE[] = "atk_normal_kinsetu_KITUKE";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_KIRISAKU[] = "atk_normal_kinsetu_KIRISAKU";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_SIBORITORU[] = "atk_normal_kinsetu_SIBORITORU";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_SIMETUKERU[] = "atk_normal_kinsetu_SIMETUKERU";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_DABURUATAKKU[] = "atk_normal_kinsetu_DABURUATAKKU";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_NIGIRITUBUSU[] = "atk_normal_kinsetu_NIGIRITUBUSU";
__declspec(align(4)) char sEffectName_atk_normal_ougi_HASAMU[] = "atk_normal_ougi_HASAMU";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_HATAKU[] = "atk_normal_kinsetu_HATAKU";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_HISSATUMAEBA[] = "atk_normal_kinsetu_HISSATUMAEBA";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_PIYOPIYOPANTI[] = "atk_normal_kinsetu_PIYOPIYOPANTI";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_HUMITUKE[] = "atk_normal_kinsetu_HUMITUKE";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_BUREIKUKUROO[] = "atk_normal_kinsetu_BUREIKUKUROO";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_MAKITUKU[] = "atk_normal_kinsetu_MAKITUKU";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_MIDAREHIKKAKI[] = "atk_normal_kinsetu_MIDAREHIKKAKI";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_MINEUTI[] = "atk_normal_kinsetu_MINEUTI";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_MEGATONKIKKU[] = "atk_normal_kinsetu_MEGATONKIKKU";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_MEGATONPANTI[] = "atk_normal_kinsetu_MEGATONPANTI";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_RENZOKUPANTI[] = "atk_normal_kinsetu_RENZOKUPANTI";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_NEKODAMASI[] = "atk_normal_kinsetu_NEKODAMASI";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_AKUMANOKISSU[] = "atk_normal_kinsetu_AKUMANOKISSU";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_TENSINOKISSU[] = "atk_normal_kinsetu_TENSINOKISSU";
__declspec(align(4)) char sEffectName_atk_kusa_kinsetu_UDDOHANMAA[] = "atk_kusa_kinsetu_UDDOHANMAA";
__declspec(align(4)) char sEffectName_atk_kusa_kinsetu_KUSAMUSUBI[] = "atk_kusa_kinsetu_KUSAMUSUBI";
__declspec(align(4)) char sEffectName_atk_kusa_kinsetu_SUITORU[] = "atk_kusa_kinsetu_SUITORU";
__declspec(align(4)) char sEffectName_atk_kusa_kinsetu_TURUNOMUTI[] = "atk_kusa_kinsetu_TURUNOMUTI";
__declspec(align(4)) char sEffectName_atk_kusa_cube_NIIDORUAAMU[] = "atk_kusa_cube_NIIDORUAAMU";
__declspec(align(4)) char sEffectName_atk_kusa_kinsetu_PAWAAWHIPPU[] = "atk_kusa_kinsetu_PAWAAWHIPPU";
__declspec(align(4)) char sEffectName_atk_kusa_kinsetu_RIIHUBUREEDO[] = "atk_kusa_kinsetu_RIIHUBUREEDO";
__declspec(align(4)) char sEffectName_atk_kusa_kinsetu_SIBIREGONA[] = "atk_kusa_kinsetu_SIBIREGONA";
__declspec(align(4)) char sEffectName_atk_kusa_ougi_KUSABUE[] = "atk_kusa_ougi_KUSABUE";
__declspec(align(4)) char sEffectName_atk_koori_ido_AISUBOORU[] = "atk_koori_ido_AISUBOORU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_KATAKUNARU[] = "atk_normal_tyuusin_KATAKUNARU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_HANERU[] = "atk_normal_tyuusin_HANERU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_WARUAGAKI[] = "atk_normal_tyuusin_WARUAGAKI";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_GAMAN[] = "atk_normal_tyuusin_GAMAN";
__declspec(align(4)) char sEffectName_atk_jimen_ougi_DOROKAKE[] = "atk_jimen_ougi_DOROKAKE";
__declspec(align(4)) char sEffectName_atk_koori_enkaku_YUKINADARE[] = "atk_koori_enkaku_YUKINADARE";
__declspec(align(4)) char sEffectName_atk_sp_ido_TEREPOOTO[] = "atk_sp_ido_TEREPOOTO";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_RIBENZI[] = "atk_battle_kinsetu_RIBENZI";
__declspec(align(4)) char sEffectName_atk_normal_ougi_NAKIGOE[] = "atk_normal_ougi_NAKIGOE";
__declspec(align(4)) char sEffectName_atk_normal_ougi_IYANAOTO[] = "atk_normal_ougi_IYANAOTO";
__declspec(align(4)) char sEffectName_atk_metal_shoot_MIRAASYOTTO[] = "atk_metal_shoot_MIRAASYOTTO";
__declspec(align(4)) char sEffectName_atk_jimen_shoot_DOROBAKUDAN[] = "atk_jimen_shoot_DOROBAKUDAN";
__declspec(align(4)) char sEffectName_atk_aku_tyuusin_WARUDAKUMI[] = "atk_aku_tyuusin_WARUDAKUMI";
__declspec(align(4)) char sEffectName_atk_iwa_tyuusin_ROKKUKATTO[] = "atk_iwa_tyuusin_ROKKUKATTO";
__declspec(align(4)) char sEffectName_atk_water_tyuusin_KARANIKOMORU[] = "atk_water_tyuusin_KARANIKOMORU";
__declspec(align(4)) char sEffectName_atk_water_tyuusin_AKUARINGU[] = "atk_water_tyuusin_AKUARINGU";
__declspec(align(4)) char sEffectName_atk_sp_tyuusin_KOSUMOPAWAA[] = "atk_sp_tyuusin_KOSUMOPAWAA";
__declspec(align(4)) char sEffectName_atk_sp_tyuusin_DOWASURE[] = "atk_sp_tyuusin_DOWASURE";
__declspec(align(4)) char sEffectName_atk_sp_tyuusin_BARIAA[] = "atk_sp_tyuusin_BARIAA";
__declspec(align(4)) char sEffectName_atk_sp_tyuusin_ZYUURYOKU[] = "atk_sp_tyuusin_ZYUURYOKU";
__declspec(align(4)) char sEffectName_atk_sp_tyuusin_MIRAAKOOTO[] = "atk_sp_tyuusin_MIRAAKOOTO";
__declspec(align(4)) char sEffectName_atk_sp_tyuusin_KOUSOKUIDOU[] = "atk_sp_tyuusin_KOUSOKUIDOU";
__declspec(align(4)) char sEffectName_atk_sp_tyuusin_HIKARINOKABE[] = "atk_sp_tyuusin_HIKARINOKABE";
__declspec(align(4)) char sEffectName_atk_sp_tyuusin_RIHUREKUTAA[] = "atk_sp_tyuusin_RIHUREKUTAA";
__declspec(align(4)) char sEffectName_atk_sp_tyuusin_YOGANOPOOZU[] = "atk_sp_tyuusin_YOGANOPOOZU";
__declspec(align(4)) char sEffectName_atk_sp_tyuusin_MEISOU[] = "atk_sp_tyuusin_MEISOU";
__declspec(align(4)) char sEffectName_atk_metal_tyuusin_TEPPEKI[] = "atk_metal_tyuusin_TEPPEKI";
__declspec(align(4)) char sEffectName_atk_metal_tyuusin_METARUBAASUTO[] = "atk_metal_tyuusin_METARUBAASUTO";
__declspec(align(4)) char sEffectName_atk_poison_tyuusin_TOKERU[] = "atk_poison_tyuusin_TOKERU";
__declspec(align(4)) char sEffectName_atk_mushi_tyuusin_BOUGYOSIREI[] = "atk_mushi_tyuusin_BOUGYOSIREI";
__declspec(align(4)) char sEffectName_atk_mushi_tyuusin_KAIHUKUSIREI[] = "atk_mushi_tyuusin_KAIHUKUSIREI";
__declspec(align(4)) char sEffectName_atk_mushi_tyuusin_HOTARUBI[] = "atk_mushi_tyuusin_HOTARUBI";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_MARUKUNARU[] = "atk_normal_tyuusin_MARUKUNARU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_KOKORONOME[] = "atk_normal_tyuusin_KOKORONOME";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_ROKKUON[] = "atk_normal_tyuusin_ROKKUON";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_KORAERU[] = "atk_normal_tyuusin_KORAERU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_MAMORU[] = "atk_normal_tyuusin_MAMORU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_IYASINOSUZU[] = "atk_normal_tyuusin_IYASINOSUZU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_RIHURESSYU[] = "atk_normal_tyuusin_RIHURESSYU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_SINPINOMAMORI[] = "atk_normal_tyuusin_SINPINOMAMORI";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_ZIKOSAISEI[] = "atk_normal_tyuusin_ZIKOSAISEI";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_TAMAGOUMI[] = "atk_normal_tyuusin_TAMAGOUMI";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_NAMAKERU[] = "atk_normal_tyuusin_NAMAKERU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_NEGAIGOTO[] = "atk_normal_tyuusin_NEGAIGOTO";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_MIRUKUNOMI[] = "atk_normal_tyuusin_MIRUKUNOMI";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_NEMURU[] = "atk_normal_tyuusin_NEMURU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_KAKUBARU[] = "atk_normal_tyuusin_KAKUBARU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_SEITYOU[] = "atk_normal_tyuusin_SEITYOU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_TURUGINOMAI[] = "atk_normal_tyuusin_TURUGINOMAI";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_TOOBOE[] = "atk_normal_tyuusin_TOOBOE";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_KIAIDAME[] = "atk_normal_tyuusin_KIAIDAME";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_HARADAIKO[] = "atk_normal_tyuusin_HARADAIKO";
__declspec(align(4)) char sEffectName_atk_electric_tyuusin_ZYUUDEN[] = "atk_electric_tyuusin_ZYUUDEN";
__declspec(align(4)) char sEffectName_atk_dragon_tyuusin_RYUUNOMAI[] = "atk_dragon_tyuusin_RYUUNOMAI";
__declspec(align(4)) char sEffectName_atk_kusa_tyuusin_AROMASERAPII[] = "atk_kusa_tyuusin_AROMASERAPII";
__declspec(align(4)) char sEffectName_atk_koori_tyuusin_KUROIKIRI[] = "atk_koori_tyuusin_KUROIKIRI";
__declspec(align(4)) char sEffectName_atk_koori_tyuusin_SIROIKIRI[] = "atk_koori_tyuusin_SIROIKIRI";
__declspec(align(4)) char sEffectName_atk_battle_tyuusin_MIKIRI[] = "atk_battle_tyuusin_MIKIRI";
__declspec(align(4)) char sEffectName_atk_battle_tyuusin_KAUNTAA[] = "atk_battle_tyuusin_KAUNTAA";
__declspec(align(4)) char sEffectName_atk_battle_tyuusin_BIRUDOAPPU[] = "atk_battle_tyuusin_BIRUDOAPPU";
__declspec(align(4)) char sEffectName_atk_hikou_tyuusin_FEZAADANSU[] = "atk_hikou_tyuusin_FEZAADANSU";
__declspec(align(4)) char sEffectName_atk_hikou_tyuusin_OIKAZE[] = "atk_hikou_tyuusin_OIKAZE";
__declspec(align(4)) char sEffectName_atk_hikou_tyuusin_HANEYASUME[] = "atk_hikou_tyuusin_HANEYASUME";
__declspec(align(4)) char sEffectName_atk_aku_kinsetu_SIPPEGAESI[] = "atk_aku_kinsetu_SIPPEGAESI";
__declspec(align(4)) char sEffectName_atk_aku_ougi_ITYAMON[] = "atk_aku_ougi_ITYAMON";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_GAMUSYARA[] = "atk_battle_kinsetu_GAMUSYARA";
__declspec(align(4)) char sEffectName_atk_battle_kinsetu_KIAIPANTI[] = "atk_battle_kinsetu_KIAIPANTI";
__declspec(align(4)) char sEffectName_atk_normal_ougi_HEBINIRAMI[] = "atk_normal_ougi_HEBINIRAMI";
__declspec(align(4)) char sEffectName_atk_normal_ougi_KANASIBARI[] = "atk_normal_ougi_KANASIBARI";
__declspec(align(4)) char sEffectName_atk_normal_ougi_KOWAIKAO[] = "atk_normal_ougi_KOWAIKAO";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_KUSUGURU[] = "atk_normal_kinsetu_KUSUGURU";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_ITAMIWAKE[] = "atk_normal_kinsetu_ITAMIWAKE";
__declspec(align(4)) char sEffectName_atk_normal_ougi_ENMAKU[] = "atk_normal_ougi_ENMAKU";
__declspec(align(4)) char sEffectName_atk_normal_ougi_AMAERU[] = "atk_normal_ougi_AMAERU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_SIPPOWOHURU[] = "atk_normal_tyuusin_SIPPOWOHURU";
__declspec(align(4)) char sEffectName_atk_normal_ougi_NIRAMITUKERU[] = "atk_normal_ougi_NIRAMITUKERU";
__declspec(align(4)) char sEffectName_atk_normal_ougi_HURASSYU[] = "atk_normal_ougi_HURASSYU";
__declspec(align(4)) char sEffectName_atk_jimen_ougi_SUNAKAKE[] = "atk_jimen_ougi_SUNAKAKE";
__declspec(align(4)) char sEffectName_atk_sp_ougi_SUPUUNMAGE[] = "atk_sp_ougi_SUPUUNMAGE";
__declspec(align(4)) char sEffectName_atk_aku_ougi_USONAKI[] = "atk_aku_ougi_USONAKI";
__declspec(align(4)) char sEffectName_atk_metal_tyuusin_KINZOKUON[] = "atk_metal_tyuusin_KINZOKUON";
__declspec(align(4)) char sEffectName_atk_kusa_ougi_WATAHOUSI[] = "atk_kusa_ougi_WATAHOUSI";
__declspec(align(4)) char sEffectName_atk_kusa_shoot_YADORIGINOTANE[] = "atk_kusa_shoot_YADORIGINOTANE";
__declspec(align(4)) char sEffectName_atk_mushi_enkaku_KUMONOSU[] = "atk_mushi_enkaku_KUMONOSU";
__declspec(align(4)) char sEffectName_atk_normal_cube_KUROIMANAZASI[] = "atk_normal_cube_KUROIMANAZASI";
__declspec(align(4)) char sEffectName_atk_normal_cube_TOOSENBOU[] = "atk_normal_cube_TOOSENBOU";
__declspec(align(4)) char sEffectName_atk_jimen_cube_ZIWARE[] = "atk_jimen_cube_ZIWARE";
__declspec(align(4)) char sEffectName_atk_koori_cube_ZETTAIREIDO[] = "atk_koori_cube_ZETTAIREIDO";
__declspec(align(4)) char sEffectName_atk_normal_ougi_HUKITOBASI[] = "atk_normal_ougi_HUKITOBASI";
__declspec(align(4)) char sEffectName_atk_normal_ougi_HOERU[] = "atk_normal_ougi_HOERU";
__declspec(align(4)) char sEffectName_atk_metal_tyuusin_HAMETUNONEGAI[] = "atk_metal_tyuusin_HAMETUNONEGAI";
__declspec(align(4)) char sEffectName_atk_sp_enkaku_MIRAIYOTI[] = "atk_sp_enkaku_MIRAIYOTI";
__declspec(align(4)) char sEffectName_atk_normal_ido_TUNODORIRU[] = "atk_normal_ido_TUNODORIRU";
__declspec(align(4)) char sEffectName_atk_mushi_ido_TONBOGAERI[] = "atk_mushi_ido_TONBOGAERI";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_HOROBINOUTA[] = "atk_normal_tyuusin_HOROBINOUTA";
__declspec(align(4)) char sEffectName_atk_sp_tyuusin_MAZIKKUKOOTO[] = "atk_sp_tyuusin_MAZIKKUKOOTO";
__declspec(align(4)) char sEffectName_atk_ghost_tyuusin_MITIDURE[] = "atk_ghost_tyuusin_MITIDURE";
__declspec(align(4)) char sEffectName_atk_aku_kinsetu_DOROBOU[] = "atk_aku_kinsetu_DOROBOU";
__declspec(align(4)) char sEffectName_atk_aku_kinsetu_HATAKIOTOSU[] = "atk_aku_kinsetu_HATAKIOTOSU";
__declspec(align(4)) char sEffectName_atk_aku_kinsetu_DAMEOSI[] = "atk_aku_kinsetu_DAMEOSI";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_TUBOWOTUKU[] = "atk_normal_kinsetu_TUBOWOTUKU";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_TEDASUKE[] = "atk_normal_kinsetu_TEDASUKE";
__declspec(align(4)) char sEffectName_atk_normal_ougi_HASAMIGIROTIN[] = "atk_normal_ougi_HASAMIGIROTIN";
__declspec(align(4)) char sEffectName_atk_normal_kinsetu_HOSIGARU[] = "atk_normal_kinsetu_HOSIGARU";
__declspec(align(4)) char sEffectName_atk_normal_ougi_HIMITUNOTIKARA[] = "atk_normal_ougi_HIMITUNOTIKARA";
__declspec(align(4)) char sEffectName_atk_mushi_kinsetu_MUSIKUI[] = "atk_mushi_kinsetu_MUSIKUI";
__declspec(align(4)) char sEffectName_atk_poison_enkaku_DOKUBISI[] = "atk_poison_enkaku_DOKUBISI";
__declspec(align(4)) char sEffectName_atk_jimen_enkaku_MAKIBISI[] = "atk_jimen_enkaku_MAKIBISI";
__declspec(align(4)) char sEffectName_atk_normal_shoot_NEKONIKOBAN[] = "atk_normal_shoot_NEKONIKOBAN";
__declspec(align(4)) char sEffectName_atk_kusa_enkaku_KOUGOUSEI[] = "atk_kusa_enkaku_KOUGOUSEI";
__declspec(align(4)) char sEffectName_atk_normal_enkaku_TUKINOHIKARI[] = "atk_normal_enkaku_TUKINOHIKARI";
__declspec(align(4)) char sEffectName_atk_normal_enkaku_ASANOHIZASI[] = "atk_normal_enkaku_ASANOHIZASI";
__declspec(align(4)) char sEffectName_atk_mushi_ougi_ITOWOHAKU[] = "atk_mushi_ougi_ITOWOHAKU";
__declspec(align(4)) char sEffectName_atk_normal_tyuusin_TIISAKUNARU[] = "atk_normal_tyuusin_TIISAKUNARU";
__declspec(align(4)) char sEffectName_atk_kusa_shoot_HAPPAKATTAA[] = "atk_kusa_shoot_HAPPAKATTAA";
__declspec(align(4)) char sEffectName_atk_hikou_shoot_EAKATTAA[] = "atk_hikou_shoot_EAKATTAA";
__declspec(align(4)) char sEffectName_atk_fire_cube_DAIMONZI[] = "atk_fire_cube_DAIMONZI";
__declspec(align(4)) char sEffectName_atk_kusa_cube_TURUNOMUTI[] = "atk_kusa_cube_TURUNOMUTI";
__declspec(align(4)) char sEffectName_atk_kusa_cube_PAWAAWHIPPU[] = "atk_kusa_cube_PAWAAWHIPPU";
__declspec(align(4)) char sEffectName_atk_normal_ido_HUMITUKE[] = "atk_normal_ido_HUMITUKE";
__declspec(align(4)) char sEffectName_atk_ghost_kinsetu_SYADOOPANTI[] = "atk_ghost_kinsetu_SYADOOPANTI";
__declspec(align(4)) char sEffectName_atk_koori_cube_KOORINOTUBUTE[] = "atk_koori_cube_KOORINOTUBUTE";
__declspec(align(4)) char sEffectName_atk_fire_ougi_OOBAAHIITO[] = "atk_fire_ougi_OOBAAHIITO";
__declspec(align(4)) char sEffectName_atk_iwa_enkaku_GANSEKIHUUZI[] = "atk_iwa_enkaku_GANSEKIHUUZI";
__declspec(align(4)) char sEffectName_atk_mushi_tyuusin_MUSINOSAZAMEKI[] = "atk_mushi_tyuusin_MUSINOSAZAMEKI";
__declspec(align(4)) char sEffectName_atk_kusa_tyuusin_SIIDOHUREA[] = "atk_kusa_tyuusin_SIIDOHUREA";
__declspec(align(4)) char sEffectName_atk_aku_enkaku_DAAKUHOORU[] = "atk_aku_enkaku_DAAKUHOORU";
__declspec(align(4)) char sEffectName_atk_sp_oneshot_DOWASURE[] = "atk_sp_oneshot_DOWASURE";
__declspec(align(4)) char sEffectName_atk_sp_oneshot_BARIAA[] = "atk_sp_oneshot_BARIAA";
__declspec(align(4)) char sEffectName_atk_sp_oneshot_SUPUUNMAGE[] = "atk_sp_oneshot_SUPUUNMAGE";
__declspec(align(4)) char sEffectName_atk_normal_oneshot_TAMAGOUMI[] = "atk_normal_oneshot_TAMAGOUMI";
__declspec(align(4)) char sEffectName_atk_normal_oneshot_MIRUKUNOMI[] = "atk_normal_oneshot_MIRUKUNOMI";
__declspec(align(4)) char sEffectName_atk_normal_oneshot_KAKUBARU[] = "atk_normal_oneshot_KAKUBARU";
__declspec(align(4)) char sEffectName_atk_sp_oneshot_HIKARINOKABE[] = "atk_sp_oneshot_HIKARINOKABE";
__declspec(align(4)) char sEffectName_atk_sp_oneshot_RIHUREKUTAA[] = "atk_sp_oneshot_RIHUREKUTAA";
__declspec(align(4)) char sEffectName_atk_sp_oneshot_MAZIKKUKOOTO[] = "atk_sp_oneshot_MAZIKKUKOOTO";
__declspec(align(4)) char sEffectName_atk_mushi_oneshot_BOUGYOSIREI[] = "atk_mushi_oneshot_BOUGYOSIREI";
__declspec(align(4)) char sEffectName_atk_iwa_oneshot_ROKKUKATTO[] = "atk_iwa_oneshot_ROKKUKATTO";
__declspec(align(4)) char sEffectName_atk_mushi_oneshot_KAIHUKUSIREI[] = "atk_mushi_oneshot_KAIHUKUSIREI";
__declspec(align(4)) char sEffectName_atk_hikou_oneshot_HANEYASUME[] = "atk_hikou_oneshot_HANEYASUME";
__declspec(align(4)) char sEffectName_atk_normal_oneshot_HARADAIKO[] = "atk_normal_oneshot_HARADAIKO";
__declspec(align(4)) char sEffectName_atk_battle_oneshot_GAMUSYARA[] = "atk_battle_oneshot_GAMUSYARA";
__declspec(align(4)) char sEffectName_atk_normal_oneshot_ITAMIWAKE[] = "atk_normal_oneshot_ITAMIWAKE";
__declspec(align(4)) char sEffectName_atk_normal_oneshot_DAIBAKUHATU[] = "atk_normal_oneshot_DAIBAKUHATU";
__declspec(align(4)) char sEffectName_atk_normal_oneshot_MARUKUNARU[] = "atk_normal_oneshot_MARUKUNARU";
__declspec(align(4)) char sEffectName_atk_water_oneshot_KARANIKOMORU[] = "atk_water_oneshot_KARANIKOMORU";
__declspec(align(4)) char sEffectName_atk_sp_oneshot_MIRAAKOOTO[] = "atk_sp_oneshot_MIRAAKOOTO";
__declspec(align(4)) char sEffectName_atk_enkaku_range[] = "atk_enkaku_range";
__declspec(align(4)) char sEffectName_atk_general_range[] = "atk_general_range";
__declspec(align(4)) char sEffectName_atk_boss_tyuusin_01[] = "atk_boss_tyuusin_01";
__declspec(align(4)) char sEffectName_atk_boss_ido_01[] = "atk_boss_ido_01";
__declspec(align(4)) char sEffectName_atk_boss_shoot_SAIKOKATTAA[] = "atk_boss_shoot_SAIKOKATTAA";
__declspec(align(4)) char sEffectName_walk_boss[] = "walk_boss";
__declspec(align(4)) char sEffectName_boss_teisi[] = "boss_teisi";
__declspec(align(4)) char sEffectName_boss_saikai[] = "boss_saikai";
__declspec(align(4)) char sEffectName_born_rival_1[] = "born_rival_1";
__declspec(align(4)) char sEffectName_born_rival_2[] = "born_rival_2";
__declspec(align(4)) char sEffectName_ROOT[] = "ROOT";
__declspec(align(4)) char sEffectName_HEAD[] = "HEAD";
__declspec(align(4)) char sEffectName_Pii_run[] = "Pii_run";
__declspec(align(4)) char sEffectName_neji[] = "neji";
__declspec(align(4)) char sEffectName_hit_s[] = "hit_s";
__declspec(align(4)) char sEffectName_hit_m[] = "hit_m";
__declspec(align(4)) char sEffectName_hit_l[] = "hit_l";
__declspec(align(4)) char sEffectName_hit_exp[] = "hit_exp";
__declspec(align(4)) char sEffectName_born[] = "born";
__declspec(align(4)) char sEffectName_die[] = "die";
__declspec(align(4)) char sEffectName_die_br[] = "die_br";
__declspec(align(4)) char sEffectName_drop[] = "drop";
__declspec(align(4)) char sEffectName_test[] = "test";
__declspec(align(4)) char sEffectName_ijo_up[] = "ijo_up";
__declspec(align(4)) char sEffectName_ijo_end[] = "ijo_end";
__declspec(align(4)) char sEffectName_kaifuku[] = "kaifuku";
__declspec(align(4)) char sEffectName_tame_01[] = "tame_01";
__declspec(align(4)) char sEffectName_tame_02[] = "tame_02";
__declspec(align(4)) char sEffectName_tame_03[] = "tame_03";
}

const EffectInfo gEffectInfos[696] = {
    {0, sEffectName_item_bakuhatu_star, sEffectName_ROOT, nullptr, 0, false},
    {1, sEffectName_item_mizu, sEffectName_ROOT, nullptr, 0, false},
    {2, sEffectName_item_honoo, sEffectName_ROOT, nullptr, 0, false},
    {3, sEffectName_item_bakuhatu_yellow, sEffectName_ROOT, nullptr, 0, false},
    {4, sEffectName_item_action, sEffectName_ROOT, nullptr, 0, false},
    {5, sEffectName_item_nioidama, sEffectName_ROOT, nullptr, 0, false},
    {6, sEffectName_item_tatumaki, sEffectName_ROOT, nullptr, 0, false},
    {7, sEffectName_item_pinwheel, sEffectName_ROOT, nullptr, 0, false},
    {8, sEffectName_item_gurasidea, sEffectName_ROOT, nullptr, 0, false},
    {9, sEffectName_Pii_kemuri, sEffectName_ROOT, nullptr, 0, false},
    {10, sEffectName_Pii_level100, sEffectName_HEAD, nullptr, 0, false},
    {11, sEffectName_Pii_haifu, sEffectName_HEAD, nullptr, 0, false},
    {12, sEffectName_Pii_run, sEffectName_ROOT, nullptr, 0, false},
    {13, sEffectName_Pii_tyakuti, sEffectName_ROOT, nullptr, 0, false},
    {14, sEffectName_Pii_attack_onpu, sEffectName_HEAD, sEffectName_HEAD, 0, false},
    {15, sEffectName_Pii_attack_normal, sEffectName_HEAD, sEffectName_HEAD, 0, false},
    {16, sEffectName_Pii_receive_normal, sEffectName_HEAD, nullptr, 0, false},
    {17, sEffectName_Pii_attack_honoo, sEffectName_HEAD, sEffectName_HEAD, 0, false},
    {18, sEffectName_Pii_receive_honoo, sEffectName_HEAD, nullptr, 0, false},
    {19, sEffectName_Pii_attack_mizu, sEffectName_HEAD, sEffectName_HEAD, 0, false},
    {20, sEffectName_Pii_receive_mizu, sEffectName_HEAD, sEffectName_HEAD, 0, false},
    {21, sEffectName_Pii_attack_denki, sEffectName_ROOT, nullptr, 0, false},
    {22, sEffectName_Pii_receive_denki, sEffectName_HEAD, nullptr, 0, false},
    {23, sEffectName_Pii_attack_kusa, sEffectName_HEAD, sEffectName_HEAD, 0, false},
    {24, sEffectName_Pii_receive_kusa, sEffectName_HEAD, nullptr, 0, false},
    {25, sEffectName_Pii_attack_esper, sEffectName_HEAD, sEffectName_HEAD, 0, false},
    {26, sEffectName_Pii_receive_esper, sEffectName_HEAD, nullptr, 0, false},
    {27, sEffectName_Pii_attack_kakutou, sEffectName_HEAD, sEffectName_HEAD, 0, false},
    {28, sEffectName_Pii_receive_kakutou, sEffectName_HEAD, nullptr, 0, false},
    {29, sEffectName_Pii_attack_doku, sEffectName_HEAD, sEffectName_HEAD, 0, false},
    {30, sEffectName_Pii_receive_doku, sEffectName_HEAD, nullptr, 0, false},
    {31, sEffectName_Pii_attack_jimen, sEffectName_ROOT, nullptr, 0, false},
    {32, sEffectName_Pii_attack_dragon, sEffectName_HEAD, sEffectName_HEAD, 0, false},
    {33, sEffectName_Pii_receive_dragon, sEffectName_ROOT, nullptr, 0, false},
    {34, sEffectName_Pii_attack_musi, sEffectName_HEAD, nullptr, 0, false},
    {35, sEffectName_Pii_receive_musi, sEffectName_HEAD, sEffectName_HEAD, 0, false},
    {36, sEffectName_Pii_attack_iwa, sEffectName_ROOT, nullptr, 0, false},
    {37, sEffectName_Pii_attack_ghost, sEffectName_HEAD, sEffectName_HEAD, 0, false},
    {38, sEffectName_Pii_attack_koori, sEffectName_HEAD, sEffectName_HEAD, 0, false},
    {39, sEffectName_Pii_attack_hagane, sEffectName_HEAD, sEffectName_HEAD, 0, false},
    {40, sEffectName_Pii_receive_hagane, sEffectName_HEAD, sEffectName_HEAD, 0, false},
    {41, sEffectName_Pii_attack_aku, sEffectName_HEAD, sEffectName_HEAD, 0, false},
    {42, sEffectName_Pii_receive_aku, sEffectName_HEAD, nullptr, 0, false},
    {43, sEffectName_stage_parade, sEffectName_ROOT, nullptr, 0, false},
    {44, sEffectName_stage_rain, sEffectName_ROOT, nullptr, 0, false},
    {45, sEffectName_stage_snow, sEffectName_ROOT, nullptr, 0, false},
    {46, sEffectName_Mii_warp, sEffectName_ROOT, nullptr, 0, false},
    {47, sEffectName_Mii_kirakira, sEffectName_ROOT, nullptr, 0, false},
    {48, sEffectName_Mii_feed, sEffectName_ROOT, sEffectName_ROOT, 0, false},
    {49, sEffectName_Pii_flash, sEffectName_ROOT, nullptr, 0, false},
    {50, sEffectName_Ball_star, sEffectName_ROOT, nullptr, 0, false},
    {51, sEffectName_Ball_kiran, sEffectName_ROOT, nullptr, 0, false},
    {52, sEffectName_Pii_syokai, sEffectName_HEAD, nullptr, 0, false},
    {53, sEffectName_Pii_suisui_jump, sEffectName_ROOT, nullptr, 0, false},
    {54, sEffectName_event_neji_star, sEffectName_neji, nullptr, 0, false},
    {55, sEffectName_event_neji_light, sEffectName_ROOT, nullptr, 0, false},
    {56, sEffectName_event_neji_get_whiteout, sEffectName_EFFECT_neji, nullptr, 0, false},
    {57, sEffectName_event_seisan, sEffectName_ROOT, nullptr, 0, false},
    {58, sEffectName_event_kamifubuki, sEffectName_EFFECT_storm, nullptr, 0, false},
    {59, sEffectName_event_boss_neji, sEffectName_neji, nullptr, 0, false},
    {60, sEffectName_event_boss, sEffectName_HEAD, nullptr, 0, false},
    {61, sEffectName_stage_rope, sEffectName_ROOT, nullptr, 0, false},
    {62, sEffectName_hit_s, sEffectName_HEAD, nullptr, 0, false},
    {63, sEffectName_hit_m, sEffectName_HEAD, nullptr, 0, false},
    {64, sEffectName_hit_l, sEffectName_HEAD, nullptr, 0, false},
    {65, sEffectName_hit_exp, sEffectName_HEAD, nullptr, 0, false},
    {66, sEffectName_hit_critical, sEffectName_HEAD, nullptr, 0, false},
    {67, sEffectName_hit_saisei, sEffectName_HEAD, nullptr, 0, false},
    {68, sEffectName_hit_bomb, sEffectName_ROOT, nullptr, 0, false},
    {69, sEffectName_hit_bomb_s, sEffectName_ROOT, nullptr, 0, false},
    {70, sEffectName_born, sEffectName_ROOT, nullptr, 0, false},
    {71, sEffectName_born_boss, sEffectName_ROOT, nullptr, 0, false},
    {72, sEffectName_die, sEffectName_ROOT, nullptr, 0, false},
    {73, sEffectName_die_normal, sEffectName_ROOT, nullptr, 0, false},
    {74, sEffectName_die_capsule, sEffectName_ROOT, nullptr, 0, false},
    {75, sEffectName_die_br, sEffectName_ROOT, nullptr, 0, false},
    {76, sEffectName_item_get, sEffectName_ROOT, nullptr, 0, false},
    {77, sEffectName_item_capsule, sEffectName_ROOT, nullptr, 0, false},
    {78, sEffectName_otamesi_out, sEffectName_ROOT, nullptr, 0, false},
    {79, sEffectName_otamesi_in, sEffectName_ROOT, nullptr, 0, false},
    {80, sEffectName_drop, sEffectName_ROOT, nullptr, 0, false},
    {81, sEffectName_drop_boss, sEffectName_ROOT, nullptr, 0, false},
    {82, sEffectName_aura_rival, sEffectName_ROOT, nullptr, 0, false},
    {83, sEffectName_aura_MYUUTUU, sEffectName_ROOT, nullptr, 0, false},
    {84, sEffectName_item_capsule_01, sEffectName_ROOT, nullptr, 0, false},
    {85, sEffectName_item_capsule_02, sEffectName_ROOT, nullptr, 0, false},
    {86, sEffectName_item_capsule_03, sEffectName_ROOT, nullptr, 0, false},
    {87, sEffectName_item_capsule_04, sEffectName_ROOT, nullptr, 0, false},
    {88, sEffectName_item_capsule_05, sEffectName_ROOT, nullptr, 0, false},
    {89, sEffectName_test, sEffectName_HEAD, nullptr, 0, true},
    {90, sEffectName_walk_fire, sEffectName_ROOT, nullptr, 0, false},
    {91, sEffectName_walk_grass, sEffectName_ROOT, nullptr, 0, false},
    {92, sEffectName_walk_sand, sEffectName_ROOT, nullptr, 0, false},
    {93, sEffectName_walk_soil, sEffectName_ROOT, nullptr, 0, false},
    {94, sEffectName_walk_water, sEffectName_ROOT, nullptr, 0, false},
    {95, sEffectName_ijo_poison, sEffectName_HEAD, nullptr, 0, false},
    {96, sEffectName_ijo_dokudoku, sEffectName_HEAD, nullptr, 0, false},
    {97, sEffectName_ijo_yakedo, sEffectName_HEAD, nullptr, 0, false},
    {98, sEffectName_ijo_mahi, sEffectName_HEAD, nullptr, 0, false},
    {99, sEffectName_ijo_koori, sEffectName_HEAD, nullptr, 0, false},
    {100, sEffectName_ijo_nemuri, sEffectName_HEAD, nullptr, 0, false},
    {101, sEffectName_ijo_konran, sEffectName_HEAD, nullptr, 0, false},
    {102, sEffectName_ijo_hirumu, sEffectName_HEAD, nullptr, 0, false},
    {103, sEffectName_ijo_up, sEffectName_HEAD, nullptr, 0, false},
    {104, sEffectName_ijo_down, sEffectName_HEAD, nullptr, 0, false},
    {105, sEffectName_ijo_kaifuku, sEffectName_HEAD, nullptr, 0, false},
    {106, sEffectName_ijo_kizetu, sEffectName_HEAD, nullptr, 0, false},
    {107, sEffectName_ijo_NEWOHARU, sEffectName_HEAD, nullptr, 0, false},
    {108, sEffectName_ijo_KOKORONOME, sEffectName_HEAD, nullptr, 0, false},
    {109, sEffectName_ijo_ROKKUON, sEffectName_HEAD, nullptr, 0, false},
    {110, sEffectName_ijo_MARUKUNARU, sEffectName_HEAD, nullptr, 0, false},
    {111, sEffectName_ijo_end, sEffectName_HEAD, nullptr, 0, false},
    {112, sEffectName_kaifuku, sEffectName_HEAD, nullptr, 0, false},
    {113, sEffectName_kaifuku_short, sEffectName_HEAD, nullptr, 0, false},
    {114, sEffectName_tame_01, sEffectName_HEAD, nullptr, 0, false},
    {115, sEffectName_tame_02, sEffectName_HEAD, nullptr, 0, false},
    {116, sEffectName_tame_03, sEffectName_HEAD, nullptr, 0, false},
    {117, sEffectName_tame_boss_01, sEffectName_HEAD, nullptr, 0, false},
    {118, sEffectName_tame_boss_02, sEffectName_HEAD, nullptr, 0, false},
    {119, sEffectName_tame_boss_03, sEffectName_HEAD, nullptr, 0, false},
    {120, sEffectName_attack_iwa, sEffectName_HEAD, nullptr, 0, false},
    {121, sEffectName_attack_honoo, sEffectName_HEAD, nullptr, 0, false},
    {122, sEffectName_atk_electric_Jet, sEffectName_HEAD, nullptr, 0, false},
    {123, sEffectName_atk_electric_Fall, sEffectName_ROOT, nullptr, 0, false},
    {124, sEffectName_atk_fire_Bomb, sEffectName_HEAD, nullptr, 0, false},
    {125, sEffectName_atk_fire_Jet, sEffectName_HEAD, nullptr, 0, false},
    {126, sEffectName_atk_fire_Jet_2, sEffectName_HEAD, nullptr, 0, false},
    {127, sEffectName_atk_fire_Jet_3, sEffectName_HEAD, nullptr, 0, false},
    {128, sEffectName_atk_fire_Shoot, sEffectName_ROOT, nullptr, 0, true},
    {129, sEffectName_atk_fire_Shoot_wide, sEffectName_ROOT, nullptr, 0, false},
    {130, sEffectName_atk_iwa_Jet, sEffectName_HEAD, nullptr, 0, false},
    {131, sEffectName_atk_iwa_Fall, sEffectName_ROOT, nullptr, 0, false},
    {132, sEffectName_atk_kusa_Jet, sEffectName_HEAD, nullptr, 0, false},
    {133, sEffectName_atk_normal_Jet_onpu, sEffectName_HEAD, nullptr, 0, false},
    {134, sEffectName_atk_poison_Jet, sEffectName_HEAD, nullptr, 0, false},
    {135, sEffectName_atk_sp_Jet, sEffectName_HEAD, nullptr, 0, false},
    {136, sEffectName_atk_water_Jet, sEffectName_HEAD, nullptr, 0, false},
    {137, sEffectName_atk_mushi_Jet, sEffectName_HEAD, nullptr, 0, false},
    {138, sEffectName_atk_kusa_shoot_HAPPAKATTAA, sEffectName_HEAD, nullptr, 0, false},
    {139, sEffectName_atk_hikou_shoot_EAKATTAA, sEffectName_HEAD, nullptr, 0, false},
    {140, sEffectName_atk_fire_cube_DAIMONZI, sEffectName_HEAD, nullptr, 4, false},
    {141, sEffectName_atk_kusa_cube_TURUNOMUTI, sEffectName_HEAD, nullptr, 4, false},
    {142, sEffectName_atk_kusa_cube_PAWAAWHIPPU, sEffectName_HEAD, nullptr, 4, false},
    {143, sEffectName_atk_normal_ido_HUMITUKE, sEffectName_HEAD, nullptr, 0, false},
    {144, sEffectName_atk_range, sEffectName_HEAD, nullptr, 0, false},
    {145, sEffectName_atk_battle_Adjacent, sEffectName_HEAD, nullptr, 0, false},
    {146, sEffectName_atk_fire_Adjacent, sEffectName_HEAD, nullptr, 0, false},
    {147, sEffectName_atk_poison_Adjacent, sEffectName_HEAD, nullptr, 0, false},
    {148, sEffectName_atk_fire_Round, sEffectName_HEAD, nullptr, 0, false},
    {149, sEffectName_atk_kusa_Round, sEffectName_HEAD, nullptr, 0, false},
    {150, sEffectName_atk_poison_Round, sEffectName_HEAD, nullptr, 0, false},
    {151, sEffectName_atk_fire_Laser, sEffectName_HEAD, nullptr, 0, false},
    {152, sEffectName_atk_normal_kinsetu_HIKKAKU, sEffectName_HEAD, nullptr, 3, false},
    {153, sEffectName_atk_normal_cube_02, sEffectName_HEAD, nullptr, 4, false},
    {154, sEffectName_atk_normal_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {155, sEffectName_atk_normal_tyuusin_ABARERU, sEffectName_ROOT, nullptr, 1, false},
    {156, sEffectName_atk_normal_ido_SINSOKU, sEffectName_HEAD, nullptr, 0, false},
    {157, sEffectName_atk_normal_shoot_02, sEffectName_HEAD, nullptr, 0, false},
    {158, sEffectName_atk_hikou_enkaku_01, sEffectName_ROOT, nullptr, 0, false},
    {159, sEffectName_atk_battle_kinsetu_MAHHAPANTI, sEffectName_HEAD, nullptr, 3, false},
    {160, sEffectName_atk_normal_cube_02, sEffectName_HEAD, nullptr, 4, false},
    {161, sEffectName_atk_normal_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {162, sEffectName_atk_battle_tyuusin_01, sEffectName_ROOT, nullptr, 1, false},
    {163, sEffectName_atk_battle_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {164, sEffectName_atk_battle_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {165, sEffectName_atk_hikou_enkaku_01, sEffectName_ROOT, nullptr, 0, false},
    {166, sEffectName_atk_hikou_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {167, sEffectName_atk_hikou_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {168, sEffectName_atk_normal_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {169, sEffectName_atk_normal_tyuusin_ABARERU, sEffectName_ROOT, nullptr, 1, false},
    {170, sEffectName_atk_hikou_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {171, sEffectName_atk_normal_shoot_02, sEffectName_HEAD, nullptr, 0, false},
    {172, sEffectName_atk_hikou_enkaku_01, sEffectName_ROOT, nullptr, 0, false},
    {173, sEffectName_atk_poison_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {174, sEffectName_atk_poison_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {175, sEffectName_atk_poison_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {176, sEffectName_atk_normal_tyuusin_ABARERU, sEffectName_ROOT, nullptr, 1, false},
    {177, sEffectName_atk_normal_ido_SINSOKU, sEffectName_HEAD, nullptr, 0, false},
    {178, sEffectName_atk_poison_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {179, sEffectName_atk_poison_enkaku_01, sEffectName_ROOT, nullptr, 0, false},
    {180, sEffectName_atk_jimen_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {181, sEffectName_atk_jimen_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {182, sEffectName_atk_normal_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {183, sEffectName_atk_jimen_tyuusin_01, sEffectName_ROOT, nullptr, 1, false},
    {184, sEffectName_atk_jimen_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {185, sEffectName_atk_jimen_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {186, sEffectName_atk_jimen_enkaku_01, sEffectName_ROOT, nullptr, 0, false},
    {187, sEffectName_atk_iwa_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {188, sEffectName_atk_iwa_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {189, sEffectName_atk_iwa_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {190, sEffectName_atk_normal_tyuusin_ABARERU, sEffectName_ROOT, nullptr, 1, false},
    {191, sEffectName_atk_iwa_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {192, sEffectName_atk_iwa_shoot_01, sEffectName_HEAD, nullptr, 6, false},
    {193, sEffectName_atk_iwa_enkaku_01, sEffectName_ROOT, nullptr, 0, false},
    {194, sEffectName_atk_mushi_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {195, sEffectName_atk_mushi_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {196, sEffectName_atk_mushi_ougi_02, sEffectName_HEAD, nullptr, 5, false},
    {197, sEffectName_atk_normal_tyuusin_ABARERU, sEffectName_ROOT, nullptr, 1, false},
    {198, sEffectName_atk_mushi_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {199, sEffectName_atk_mushi_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {200, sEffectName_atk_hikou_enkaku_01, sEffectName_ROOT, nullptr, 0, false},
    {201, sEffectName_atk_ghost_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {202, sEffectName_atk_ghost_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {203, sEffectName_atk_normal_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {204, sEffectName_atk_normal_tyuusin_ABARERU, sEffectName_ROOT, nullptr, 1, false},
    {205, sEffectName_atk_ghost_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {206, sEffectName_atk_ghost_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {207, sEffectName_atk_ghost_enkaku_01, sEffectName_ROOT, nullptr, 0, false},
    {208, sEffectName_atk_metal_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {209, sEffectName_atk_metal_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {210, sEffectName_atk_normal_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {211, sEffectName_atk_normal_tyuusin_ABARERU, sEffectName_ROOT, nullptr, 1, false},
    {212, sEffectName_atk_normal_ido_SINSOKU, sEffectName_HEAD, nullptr, 0, false},
    {213, sEffectName_atk_metal_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {214, sEffectName_atk_hikou_enkaku_01, sEffectName_ROOT, nullptr, 0, false},
    {215, sEffectName_atk_fire_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {216, sEffectName_atk_fire_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {217, sEffectName_atk_fire_ougi_SEINARUHONOO, sEffectName_HEAD, nullptr, 5, false},
    {218, sEffectName_atk_fire_tyuusin_01, sEffectName_ROOT, nullptr, 1, false},
    {219, sEffectName_atk_fire_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {220, sEffectName_atk_fire_shoot, sEffectName_HEAD, nullptr, 0, false},
    {221, sEffectName_atk_hikou_enkaku_01, sEffectName_ROOT, nullptr, 0, false},
    {222, sEffectName_atk_water_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {223, sEffectName_atk_water_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {224, sEffectName_atk_water_ougi_AWA, sEffectName_HEAD, nullptr, 5, false},
    {225, sEffectName_atk_normal_tyuusin_ABARERU, sEffectName_ROOT, nullptr, 1, false},
    {226, sEffectName_atk_water_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {227, sEffectName_atk_water_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {228, sEffectName_atk_water_enkaku_01, sEffectName_ROOT, nullptr, 0, false},
    {229, sEffectName_atk_kusa_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {230, sEffectName_atk_kusa_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {231, sEffectName_atk_kusa_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {232, sEffectName_atk_kusa_tyuusin_01, sEffectName_ROOT, nullptr, 1, false},
    {233, sEffectName_atk_normal_ido_SINSOKU, sEffectName_HEAD, nullptr, 0, false},
    {234, sEffectName_atk_kusa_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {235, sEffectName_atk_kusa_enkaku_01, sEffectName_ROOT, nullptr, 0, false},
    {236, sEffectName_atk_electric_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {237, sEffectName_atk_electric_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {238, sEffectName_atk_electric_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {239, sEffectName_atk_electric_tyuusin_01, sEffectName_ROOT, nullptr, 1, false},
    {240, sEffectName_atk_electric_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {241, sEffectName_atk_electric_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {242, sEffectName_atk_electric_enkaku_01, sEffectName_ROOT, nullptr, 0, false},
    {243, sEffectName_atk_normal_kinsetu_HIKKAKU, sEffectName_HEAD, nullptr, 3, false},
    {244, sEffectName_atk_sp_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {245, sEffectName_atk_sp_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {246, sEffectName_atk_normal_tyuusin_ABARERU, sEffectName_ROOT, nullptr, 1, false},
    {247, sEffectName_atk_sp_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {248, sEffectName_atk_sp_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {249, sEffectName_atk_sp_enkaku_01, sEffectName_ROOT, nullptr, 0, false},
    {250, sEffectName_atk_koori_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {251, sEffectName_atk_koori_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {252, sEffectName_atk_koori_ougi_02, sEffectName_HEAD, nullptr, 5, false},
    {253, sEffectName_atk_normal_tyuusin_ABARERU, sEffectName_ROOT, nullptr, 1, false},
    {254, sEffectName_atk_normal_ido_SINSOKU, sEffectName_HEAD, nullptr, 0, false},
    {255, sEffectName_atk_koori_shoot_02, sEffectName_HEAD, nullptr, 0, false},
    {256, sEffectName_atk_hikou_enkaku_01, sEffectName_ROOT, nullptr, 0, false},
    {257, sEffectName_atk_dragon_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {258, sEffectName_atk_dragon_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {259, sEffectName_atk_normal_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {260, sEffectName_atk_dragon_tyuusin_01, sEffectName_ROOT, nullptr, 1, false},
    {261, sEffectName_atk_dragon_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {262, sEffectName_atk_normal_shoot_02, sEffectName_HEAD, nullptr, 0, false},
    {263, sEffectName_atk_dragon_enkaku_01, sEffectName_ROOT, nullptr, 0, false},
    {264, sEffectName_atk_aku_kinsetu_HUIUTI, sEffectName_HEAD, nullptr, 3, false},
    {265, sEffectName_atk_aku_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {266, sEffectName_atk_normal_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {267, sEffectName_atk_normal_tyuusin_ABARERU, sEffectName_ROOT, nullptr, 1, false},
    {268, sEffectName_atk_aku_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {269, sEffectName_atk_normal_shoot_02, sEffectName_HEAD, nullptr, 0, false},
    {270, sEffectName_atk_hikou_enkaku_01, sEffectName_ROOT, nullptr, 0, false},
    {271, sEffectName_atk_water_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {272, sEffectName_atk_water_ido_02, sEffectName_HEAD, nullptr, 0, false},
    {273, sEffectName_atk_sp_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {274, sEffectName_atk_normal_ido_SINSOKU, sEffectName_HEAD, nullptr, 0, false},
    {275, sEffectName_atk_normal_ido_ZUTUKI, sEffectName_HEAD, nullptr, 0, false},
    {276, sEffectName_atk_normal_ido_SUTEMITAKKURU, sEffectName_HEAD, nullptr, 0, false},
    {277, sEffectName_atk_normal_ido_TAIATARI, sEffectName_HEAD, nullptr, 0, false},
    {278, sEffectName_atk_normal_kinsetu_TATAKITUKERU, sEffectName_HEAD, nullptr, 3, false},
    {279, sEffectName_atk_normal_ido_TUNODETUKU, sEffectName_HEAD, nullptr, 0, false},
    {280, sEffectName_atk_normal_ido_DENKOUSEKKA, sEffectName_HEAD, nullptr, 0, false},
    {281, sEffectName_atk_normal_ido_TOSSIN, sEffectName_HEAD, nullptr, 0, false},
    {282, sEffectName_atk_normal_ido_NOSIKAKARI, sEffectName_HEAD, nullptr, 0, false},
    {283, sEffectName_atk_normal_ido_MIDAREDUKI, sEffectName_HEAD, nullptr, 0, false},
    {284, sEffectName_atk_normal_ido_ROKETTOZUTUKI, sEffectName_HEAD, nullptr, 0, false},
    {285, sEffectName_atk_normal_ido_ROKKUKURAIMU, sEffectName_HEAD, nullptr, 0, false},
    {286, sEffectName_atk_mushi_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {287, sEffectName_atk_jimen_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {288, sEffectName_atk_iwa_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {289, sEffectName_atk_iwa_ido_02, sEffectName_HEAD, nullptr, 0, false},
    {290, sEffectName_atk_hikou_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {291, sEffectName_atk_hikou_ido_02, sEffectName_HEAD, nullptr, 0, false},
    {292, sEffectName_atk_hikou_ido_03, sEffectName_HEAD, nullptr, 0, false},
    {293, sEffectName_atk_hikou_ido_04, sEffectName_HEAD, nullptr, 0, false},
    {294, sEffectName_atk_hikou_ido_05, sEffectName_HEAD, nullptr, 0, false},
    {295, sEffectName_atk_hikou_ido_06, sEffectName_HEAD, nullptr, 0, false},
    {296, sEffectName_atk_ghost_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {297, sEffectName_atk_ghost_ido_02, sEffectName_HEAD, nullptr, 0, false},
    {298, sEffectName_atk_fire_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {299, sEffectName_atk_electric_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {300, sEffectName_atk_dragon_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {301, sEffectName_atk_battle_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {302, sEffectName_atk_battle_ido_02, sEffectName_HEAD, nullptr, 0, false},
    {303, sEffectName_atk_aku_ido_01, sEffectName_HEAD, nullptr, 0, false},
    {304, sEffectName_atk_water_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {305, sEffectName_atk_sp_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {306, sEffectName_atk_sp_shoot_02, sEffectName_HEAD, nullptr, 0, false},
    {307, sEffectName_atk_poison_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {308, sEffectName_atk_poison_shoot_02, sEffectName_HEAD, nullptr, 0, false},
    {309, sEffectName_atk_poison_shoot_03, sEffectName_HEAD, nullptr, 0, false},
    {310, sEffectName_atk_poison_cube_HEDOROKOUGEKI, sEffectName_HEAD, nullptr, 4, false},
    {311, sEffectName_atk_normal_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {312, sEffectName_atk_normal_shoot_02, sEffectName_HEAD, nullptr, 0, false},
    {313, sEffectName_atk_normal_shoot_03, sEffectName_HEAD, nullptr, 0, false},
    {314, sEffectName_atk_normal_shoot_04, sEffectName_HEAD, nullptr, 0, false},
    {315, sEffectName_atk_normal_shoot_05, sEffectName_HEAD, nullptr, 0, false},
    {316, sEffectName_atk_normal_shoot_06, sEffectName_HEAD, nullptr, 0, false},
    {317, sEffectName_atk_mushi_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {318, sEffectName_atk_mushi_shoot_02, sEffectName_HEAD, nullptr, 0, false},
    {319, sEffectName_atk_metal_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {320, sEffectName_atk_metal_shoot_02, sEffectName_HEAD, nullptr, 0, false},
    {321, sEffectName_atk_kusa_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {322, sEffectName_atk_kusa_shoot_02, sEffectName_HEAD, nullptr, 0, false},
    {323, sEffectName_atk_kusa_shoot_03, sEffectName_HEAD, nullptr, 0, false},
    {324, sEffectName_atk_kusa_shoot_04, sEffectName_HEAD, nullptr, 0, false},
    {325, sEffectName_atk_koori_shoot_02, sEffectName_HEAD, nullptr, 0, false},
    {326, sEffectName_atk_koori_shoot_03, sEffectName_HEAD, nullptr, 0, false},
    {327, sEffectName_atk_jimen_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {328, sEffectName_atk_electric_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {329, sEffectName_atk_iwa_shoot_01, sEffectName_HEAD, nullptr, 6, false},
    {330, sEffectName_atk_iwa_shoot_02, sEffectName_HEAD, nullptr, 6, false},
    {331, sEffectName_atk_ghost_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {332, sEffectName_atk_battle_shoot_01, sEffectName_HEAD, nullptr, 0, false},
    {333, sEffectName_atk_battle_shoot_02, sEffectName_HEAD, nullptr, 0, false},
    {334, sEffectName_atk_kusa_jibun_01, sEffectName_ROOT, nullptr, 0, false},
    {335, sEffectName_atk_battle_tyuusin_01, sEffectName_ROOT, nullptr, 1, false},
    {336, sEffectName_atk_battle_tyuusin_02, sEffectName_ROOT, nullptr, 1, false},
    {337, sEffectName_atk_kusa_tyuusin_01, sEffectName_ROOT, nullptr, 1, false},
    {338, sEffectName_atk_kusa_tyuusin_02, sEffectName_ROOT, nullptr, 1, false},
    {339, sEffectName_atk_jimen_tyuusin_01, sEffectName_ROOT, nullptr, 1, false},
    {340, sEffectName_atk_jimen_tyuusin_02, sEffectName_ROOT, nullptr, 1, false},
    {341, sEffectName_atk_electric_tyuusin_01, sEffectName_ROOT, nullptr, 1, false},
    {342, sEffectName_atk_electric_tyuusin_02, sEffectName_ROOT, nullptr, 1, false},
    {343, sEffectName_atk_dragon_tyuusin_01, sEffectName_ROOT, nullptr, 1, false},
    {344, sEffectName_atk_dragon_tyuusin_02, sEffectName_ROOT, nullptr, 1, false},
    {345, sEffectName_atk_fire_tyuusin_02, sEffectName_ROOT, nullptr, 1, false},
    {346, sEffectName_atk_fire_tyuusin_03, sEffectName_ROOT, nullptr, 1, false},
    {347, sEffectName_atk_aku_kinsetu_TUZIGIRI, sEffectName_HEAD, nullptr, 3, false},
    {348, sEffectName_atk_aku_kinsetu_HUIUTI, sEffectName_HEAD, nullptr, 3, false},
    {349, sEffectName_atk_aku_kinsetu_OIUTI, sEffectName_HEAD, nullptr, 3, false},
    {350, sEffectName_atk_aku_kinsetu_OSIOKI, sEffectName_HEAD, nullptr, 3, false},
    {351, sEffectName_atk_aku_kinsetu_KAMITUKU, sEffectName_HEAD, nullptr, 3, false},
    {352, sEffectName_atk_aku_kinsetu_KAMIKUDAKU, sEffectName_HEAD, nullptr, 3, false},
    {353, sEffectName_atk_kusa_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {354, sEffectName_atk_ghost_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {355, sEffectName_atk_ghost_kinsetu_02, sEffectName_HEAD, nullptr, 3, false},
    {356, sEffectName_atk_ghost_kinsetu_03, sEffectName_HEAD, nullptr, 3, false},
    {357, sEffectName_atk_koori_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {358, sEffectName_atk_koori_kinsetu_02, sEffectName_HEAD, nullptr, 3, false},
    {359, sEffectName_atk_jimen_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {360, sEffectName_atk_jimen_kinsetu_02, sEffectName_HEAD, nullptr, 3, false},
    {361, sEffectName_atk_electric_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {362, sEffectName_atk_electric_kinsetu_02, sEffectName_HEAD, nullptr, 3, false},
    {363, sEffectName_atk_electric_kinsetu_03, sEffectName_HEAD, nullptr, 3, false},
    {364, sEffectName_atk_poison_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {365, sEffectName_atk_poison_kinsetu_02, sEffectName_HEAD, nullptr, 3, false},
    {366, sEffectName_atk_poison_kinsetu_03, sEffectName_HEAD, nullptr, 3, false},
    {367, sEffectName_atk_poison_kinsetu_04, sEffectName_HEAD, nullptr, 3, false},
    {368, sEffectName_atk_poison_kinsetu_05, sEffectName_HEAD, nullptr, 3, false},
    {369, sEffectName_atk_dragon_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {370, sEffectName_atk_metal_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {371, sEffectName_atk_metal_kinsetu_02, sEffectName_HEAD, nullptr, 3, false},
    {372, sEffectName_atk_metal_kinsetu_03, sEffectName_HEAD, nullptr, 3, false},
    {373, sEffectName_atk_metal_cube_BARETTOPANTI, sEffectName_HEAD, nullptr, 4, false},
    {374, sEffectName_atk_metal_kinsetu_05, sEffectName_HEAD, nullptr, 3, false},
    {375, sEffectName_atk_hikou_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {376, sEffectName_atk_hikou_kinsetu_02, sEffectName_HEAD, nullptr, 3, false},
    {377, sEffectName_atk_hikou_kinsetu_03, sEffectName_HEAD, nullptr, 3, false},
    {378, sEffectName_atk_fire_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {379, sEffectName_atk_fire_kinsetu_02, sEffectName_HEAD, nullptr, 3, false},
    {380, sEffectName_atk_fire_kinsetu_03, sEffectName_HEAD, nullptr, 3, false},
    {381, sEffectName_atk_fire_kinsetu_04, sEffectName_HEAD, nullptr, 3, false},
    {382, sEffectName_atk_water_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {383, sEffectName_atk_water_ougi_KARADEHASAMU, sEffectName_ROOT, nullptr, 5, false},
    {384, sEffectName_atk_water_kinsetu_03, sEffectName_HEAD, nullptr, 3, false},
    {385, sEffectName_atk_mushi_kinsetu_01, sEffectName_HEAD, nullptr, 3, false},
    {386, sEffectName_atk_mushi_kinsetu_02, sEffectName_HEAD, nullptr, 3, false},
    {387, sEffectName_atk_mushi_kinsetu_03, sEffectName_HEAD, nullptr, 3, false},
    {388, sEffectName_atk_iwa_enkaku_01, sEffectName_ROOT, nullptr, 2, false},
    {389, sEffectName_atk_iwa_enkaku_02, sEffectName_ROOT, nullptr, 2, false},
    {390, sEffectName_atk_iwa_enkaku_03, sEffectName_ROOT, nullptr, 2, false},
    {391, sEffectName_atk_sp_enkaku_01, sEffectName_ROOT, nullptr, 2, false},
    {392, sEffectName_atk_sp_enkaku_02, sEffectName_ROOT, nullptr, 2, false},
    {393, sEffectName_atk_sp_enkaku_03, sEffectName_ROOT, nullptr, 2, false},
    {394, sEffectName_atk_kusa_enkaku_01, sEffectName_ROOT, nullptr, 2, false},
    {395, sEffectName_atk_ghost_enkaku_01, sEffectName_ROOT, nullptr, 2, false},
    {396, sEffectName_atk_jimen_enkaku_01, sEffectName_ROOT, nullptr, 2, false},
    {397, sEffectName_atk_jimen_enkaku_02, sEffectName_ROOT, nullptr, 2, false},
    {398, sEffectName_atk_electric_enkaku_01, sEffectName_ROOT, nullptr, 2, false},
    {399, sEffectName_atk_electric_enkaku_02, sEffectName_ROOT, nullptr, 2, false},
    {400, sEffectName_atk_poison_enkaku_01, sEffectName_ROOT, nullptr, 2, false},
    {401, sEffectName_atk_dragon_enkaku_01, sEffectName_ROOT, nullptr, 2, false},
    {402, sEffectName_atk_dragon_enkaku_02, sEffectName_ROOT, nullptr, 2, false},
    {403, sEffectName_atk_water_enkaku_01, sEffectName_ROOT, nullptr, 2, false},
    {404, sEffectName_atk_hikou_enkaku_01, sEffectName_ROOT, nullptr, 2, false},
    {405, sEffectName_atk_aku_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {406, sEffectName_atk_iwa_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {407, sEffectName_atk_iwa_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {408, sEffectName_atk_sp_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {409, sEffectName_atk_sp_ougi_02, sEffectName_HEAD, nullptr, 5, false},
    {410, sEffectName_atk_sp_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {411, sEffectName_atk_sp_cube_02, sEffectName_HEAD, nullptr, 4, false},
    {412, sEffectName_atk_sp_cube_03, sEffectName_HEAD, nullptr, 4, false},
    {413, sEffectName_atk_kusa_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {414, sEffectName_atk_kusa_ougi_02, sEffectName_HEAD, nullptr, 5, false},
    {415, sEffectName_atk_kusa_ougi_03, sEffectName_HEAD, nullptr, 5, false},
    {416, sEffectName_atk_kusa_ougi_04, sEffectName_HEAD, nullptr, 5, false},
    {417, sEffectName_atk_kusa_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {418, sEffectName_atk_ghost_cube_AYASIIKAZE, sEffectName_HEAD, nullptr, 4, false},
    {419, sEffectName_atk_ghost_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {420, sEffectName_atk_koori_cube_KOGOERUKAZE, sEffectName_HEAD, nullptr, 4, false},
    {421, sEffectName_atk_koori_ougi_02, sEffectName_HEAD, nullptr, 5, false},
    {422, sEffectName_atk_koori_cube_HUBUKI, sEffectName_HEAD, nullptr, 4, false},
    {423, sEffectName_atk_koori_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {424, sEffectName_atk_koori_cube_02, sEffectName_HEAD, nullptr, 4, false},
    {425, sEffectName_atk_jimen_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {426, sEffectName_atk_electric_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {427, sEffectName_atk_electric_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {428, sEffectName_atk_poison_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {429, sEffectName_atk_poison_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {430, sEffectName_atk_poison_cube_02, sEffectName_HEAD, nullptr, 4, false},
    {431, sEffectName_atk_dragon_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {432, sEffectName_atk_dragon_cube_02, sEffectName_HEAD, nullptr, 4, false},
    {433, sEffectName_atk_dragon_cube_03, sEffectName_HEAD, nullptr, 4, false},
    {434, sEffectName_atk_dragon_cube_04, sEffectName_HEAD, nullptr, 4, false},
    {435, sEffectName_atk_normal_ougi_01, sEffectName_HEAD, nullptr, 5, false},
    {436, sEffectName_atk_normal_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {437, sEffectName_atk_normal_cube_02, sEffectName_HEAD, nullptr, 4, false},
    {438, sEffectName_atk_normal_cube_03, sEffectName_HEAD, nullptr, 4, false},
    {439, sEffectName_atk_metal_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {440, sEffectName_atk_hikou_cube_EAKATTAA, sEffectName_HEAD, nullptr, 4, false},
    {441, sEffectName_atk_hikou_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {442, sEffectName_atk_fire_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {443, sEffectName_atk_water_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {444, sEffectName_atk_water_cube_02, sEffectName_HEAD, nullptr, 4, false},
    {445, sEffectName_atk_water_cube_03, sEffectName_HEAD, nullptr, 4, false},
    {446, sEffectName_atk_water_cube_04, sEffectName_HEAD, nullptr, 4, false},
    {447, sEffectName_atk_water_cube_05, sEffectName_HEAD, nullptr, 4, false},
    {448, sEffectName_atk_mushi_cube_GINIRONOKAZE, sEffectName_HEAD, nullptr, 4, false},
    {449, sEffectName_atk_mushi_ougi_02, sEffectName_HEAD, nullptr, 5, false},
    {450, sEffectName_atk_mushi_cube_01, sEffectName_HEAD, nullptr, 4, false},
    {451, sEffectName_atk_electric_kinsetu_KAMINARIPANTI, sEffectName_HEAD, nullptr, 3, false},
    {452, sEffectName_atk_fire_kinsetu_ONIBI, sEffectName_HEAD, nullptr, 3, false},
    {453, sEffectName_atk_normal_tyuusin_AKUBI, sEffectName_ROOT, nullptr, 1, false},
    {454, sEffectName_atk_normal_ougi_AKUBI, sEffectName_HEAD, nullptr, 5, false},
    {455, sEffectName_atk_normal_tyuusin_UTAU, sEffectName_ROOT, nullptr, 1, false},
    {456, sEffectName_atk_normal_tyuusin_HURAHURADANSU, sEffectName_ROOT, nullptr, 1, false},
    {457, sEffectName_atk_kusa_ougi_KINOKONOHOUSI, sEffectName_HEAD, nullptr, 5, false},
    {458, sEffectName_atk_ghost_ougi_AYASIIHIKARI, sEffectName_HEAD, nullptr, 5, false},
    {459, sEffectName_atk_kusa_ougi_NEMURIGONA, sEffectName_HEAD, nullptr, 5, false},
    {460, sEffectName_atk_normal_ougi_IBARU, sEffectName_HEAD, nullptr, 5, false},
    {461, sEffectName_atk_aku_enkaku_ODATERU, sEffectName_ROOT, nullptr, 2, false},
    {462, sEffectName_atk_normal_cube_TYOUONPA, sEffectName_HEAD, nullptr, 4, false},
    {463, sEffectName_atk_fire_ougi_SEINARUHONOO, sEffectName_HEAD, nullptr, 5, false},
    {464, sEffectName_atk_fire_ougi_DAIMONZI, sEffectName_HEAD, nullptr, 5, false},
    {465, sEffectName_atk_fire_cube_NEPPUU, sEffectName_HEAD, nullptr, 4, false},
    {466, sEffectName_atk_fire_enkaku_HUNKA, sEffectName_ROOT, nullptr, 2, false},
    {467, sEffectName_atk_water_ougi_AWA, sEffectName_HEAD, nullptr, 5, false},
    {468, sEffectName_atk_water_enkaku_SIOHUKI, sEffectName_ROOT, nullptr, 2, false},
    {469, sEffectName_atk_water_cube_DAKURYUU, sEffectName_HEAD, nullptr, 4, false},
    {470, sEffectName_atk_water_ougi_MIZUNOHADOU, sEffectName_HEAD, nullptr, 5, false},
    {471, sEffectName_atk_water_enkaku_UZUSIO, sEffectName_ROOT, nullptr, 2, false},
    {472, sEffectName_atk_fire_enkaku_HONOONOUZU, sEffectName_ROOT, nullptr, 2, false},
    {473, sEffectName_atk_fire_enkaku_MAGUMASUTOOMU, sEffectName_ROOT, nullptr, 2, false},
    {474, sEffectName_atk_fire_cube_BURASUTOBAAN, sEffectName_HEAD, nullptr, 4, false},
    {475, sEffectName_atk_water_cube_NAMINORI, sEffectName_HEAD, nullptr, 4, false},
    {476, sEffectName_atk_normal_tyuusin_ABARERU, sEffectName_ROOT, nullptr, 1, false},
    {477, sEffectName_atk_normal_tyuusin_KAMAITATI, sEffectName_ROOT, nullptr, 1, false},
    {478, sEffectName_atk_normal_tyuusin_KOUSOKUSUPIN, sEffectName_ROOT, nullptr, 1, false},
    {479, sEffectName_atk_normal_tyuusin_SAWAGU, sEffectName_ROOT, nullptr, 1, false},
    {480, sEffectName_atk_normal_tyuusin_ZITABATA, sEffectName_ROOT, nullptr, 1, false},
    {481, sEffectName_atk_normal_tyuusin_ZIBAKU, sEffectName_ROOT, nullptr, 1, false},
    {482, sEffectName_atk_normal_tyuusin_MEZAMERUPAWAA, sEffectName_ROOT, nullptr, 1, false},
    {483, sEffectName_atk_normal_tyuusin_DAIBAKUHATU, sEffectName_ROOT, nullptr, 1, false},
    {484, sEffectName_atk_battle_kinsetu_MAWASIGERI, sEffectName_HEAD, nullptr, 3, false},
    {485, sEffectName_atk_battle_kinsetu_AAMUHANMAA, sEffectName_HEAD, nullptr, 3, false},
    {486, sEffectName_atk_battle_kinsetu_ATEMINAGE, sEffectName_HEAD, nullptr, 3, false},
    {487, sEffectName_atk_battle_kinsetu_IWAKUDAKI, sEffectName_HEAD, nullptr, 3, false},
    {488, sEffectName_atk_battle_kinsetu_INFAITO, sEffectName_HEAD, nullptr, 3, false},
    {489, sEffectName_atk_battle_kinsetu_KARATETYOPPU, sEffectName_HEAD, nullptr, 3, false},
    {490, sEffectName_atk_battle_kinsetu_KAWARAWARI, sEffectName_HEAD, nullptr, 3, false},
    {491, sEffectName_atk_battle_kinsetu_KUROSUTYOPPU, sEffectName_HEAD, nullptr, 3, false},
    {492, sEffectName_atk_battle_kinsetu_KETAGURI, sEffectName_HEAD, nullptr, 3, false},
    {493, sEffectName_atk_battle_kinsetu_SINKUUHA, sEffectName_HEAD, nullptr, 3, false},
    {494, sEffectName_atk_battle_ido_SUKAIAPPAA, sEffectName_HEAD, nullptr, 0, false},
    {495, sEffectName_atk_battle_kinsetu_TIKYUUNAGE, sEffectName_HEAD, nullptr, 3, false},
    {496, sEffectName_atk_battle_kinsetu_TUPPARI, sEffectName_HEAD, nullptr, 3, false},
    {497, sEffectName_atk_battle_kinsetu_TORIPURUKIKKU, sEffectName_HEAD, nullptr, 3, false},
    {498, sEffectName_atk_battle_kinsetu_DOREINPANTI, sEffectName_HEAD, nullptr, 3, false},
    {499, sEffectName_atk_battle_kinsetu_NIDOGERI, sEffectName_HEAD, nullptr, 3, false},
    {500, sEffectName_atk_battle_kinsetu_BAKADIKARA, sEffectName_HEAD, nullptr, 3, false},
    {501, sEffectName_atk_battle_kinsetu_BAKURETUPANTI, sEffectName_HEAD, nullptr, 3, false},
    {502, sEffectName_atk_battle_kinsetu_HAKKEI, sEffectName_HEAD, nullptr, 3, false},
    {503, sEffectName_atk_battle_kinsetu_MAHHAPANTI, sEffectName_HEAD, nullptr, 3, false},
    {504, sEffectName_atk_battle_kinsetu_MEZAMASIBINTA, sEffectName_HEAD, nullptr, 3, false},
    {505, sEffectName_atk_normal_kinsetu_IAIGIRI, sEffectName_HEAD, nullptr, 3, false},
    {506, sEffectName_atk_normal_kinsetu_IKARI, sEffectName_HEAD, nullptr, 3, false},
    {507, sEffectName_atk_normal_kinsetu_IKARINOMAEBA, sEffectName_HEAD, nullptr, 3, false},
    {508, sEffectName_atk_normal_kinsetu_OUHUKUBINTA, sEffectName_HEAD, nullptr, 3, false},
    {509, sEffectName_atk_normal_kinsetu_KAIRIKI, sEffectName_HEAD, nullptr, 3, false},
    {510, sEffectName_atk_normal_kinsetu_KARAGENKI, sEffectName_HEAD, nullptr, 3, false},
    {511, sEffectName_atk_normal_ougi_KARAMITUKU, sEffectName_HEAD, nullptr, 5, false},
    {512, sEffectName_atk_normal_kinsetu_GIGAINPAKUTO, sEffectName_HEAD, nullptr, 3, false},
    {513, sEffectName_atk_normal_kinsetu_KITUKE, sEffectName_HEAD, nullptr, 3, false},
    {514, sEffectName_atk_normal_kinsetu_KIRISAKU, sEffectName_HEAD, nullptr, 3, false},
    {515, sEffectName_atk_normal_kinsetu_SIBORITORU, sEffectName_HEAD, nullptr, 3, false},
    {516, sEffectName_atk_normal_kinsetu_SIMETUKERU, sEffectName_HEAD, nullptr, 3, false},
    {517, sEffectName_atk_normal_kinsetu_DABURUATAKKU, sEffectName_HEAD, nullptr, 3, false},
    {518, sEffectName_atk_normal_kinsetu_NIGIRITUBUSU, sEffectName_HEAD, nullptr, 3, false},
    {519, sEffectName_atk_normal_ougi_HASAMU, sEffectName_HEAD, nullptr, 5, false},
    {520, sEffectName_atk_normal_kinsetu_HATAKU, sEffectName_HEAD, nullptr, 3, false},
    {521, sEffectName_atk_normal_kinsetu_HIKKAKU, sEffectName_HEAD, nullptr, 3, false},
    {522, sEffectName_atk_normal_kinsetu_HISSATUMAEBA, sEffectName_HEAD, nullptr, 3, false},
    {523, sEffectName_atk_normal_kinsetu_PIYOPIYOPANTI, sEffectName_HEAD, nullptr, 3, false},
    {524, sEffectName_atk_normal_kinsetu_HUMITUKE, sEffectName_HEAD, nullptr, 3, false},
    {525, sEffectName_atk_normal_kinsetu_BUREIKUKUROO, sEffectName_HEAD, nullptr, 3, false},
    {526, sEffectName_atk_normal_kinsetu_MAKITUKU, sEffectName_HEAD, nullptr, 3, false},
    {527, sEffectName_atk_normal_kinsetu_MIDAREHIKKAKI, sEffectName_HEAD, nullptr, 3, false},
    {528, sEffectName_atk_normal_kinsetu_MINEUTI, sEffectName_HEAD, nullptr, 3, false},
    {529, sEffectName_atk_normal_kinsetu_MEGATONKIKKU, sEffectName_HEAD, nullptr, 3, false},
    {530, sEffectName_atk_normal_kinsetu_MEGATONPANTI, sEffectName_HEAD, nullptr, 3, false},
    {531, sEffectName_atk_normal_kinsetu_RENZOKUPANTI, sEffectName_HEAD, nullptr, 3, false},
    {532, sEffectName_atk_normal_kinsetu_NEKODAMASI, sEffectName_HEAD, nullptr, 3, false},
    {533, sEffectName_atk_normal_kinsetu_AKUMANOKISSU, sEffectName_HEAD, nullptr, 3, false},
    {534, sEffectName_atk_normal_kinsetu_TENSINOKISSU, sEffectName_HEAD, nullptr, 3, false},
    {535, sEffectName_atk_kusa_kinsetu_UDDOHANMAA, sEffectName_HEAD, nullptr, 3, false},
    {536, sEffectName_atk_kusa_kinsetu_KUSAMUSUBI, sEffectName_HEAD, nullptr, 3, false},
    {537, sEffectName_atk_kusa_kinsetu_SUITORU, sEffectName_HEAD, nullptr, 3, false},
    {538, sEffectName_atk_kusa_kinsetu_TURUNOMUTI, sEffectName_HEAD, nullptr, 3, false},
    {539, sEffectName_atk_kusa_cube_NIIDORUAAMU, sEffectName_HEAD, nullptr, 4, false},
    {540, sEffectName_atk_kusa_kinsetu_PAWAAWHIPPU, sEffectName_HEAD, nullptr, 3, false},
    {541, sEffectName_atk_kusa_kinsetu_RIIHUBUREEDO, sEffectName_HEAD, nullptr, 3, false},
    {542, sEffectName_atk_kusa_kinsetu_SIBIREGONA, sEffectName_HEAD, nullptr, 3, false},
    {543, sEffectName_atk_kusa_ougi_KUSABUE, sEffectName_HEAD, nullptr, 5, false},
    {544, sEffectName_atk_koori_ido_AISUBOORU, sEffectName_HEAD, nullptr, 0, false},
    {545, sEffectName_atk_normal_tyuusin_KATAKUNARU, sEffectName_ROOT, nullptr, 1, false},
    {546, sEffectName_atk_normal_tyuusin_HANERU, sEffectName_ROOT, nullptr, 1, false},
    {547, sEffectName_atk_normal_tyuusin_WARUAGAKI, sEffectName_ROOT, nullptr, 1, false},
    {548, sEffectName_atk_normal_tyuusin_GAMAN, sEffectName_ROOT, nullptr, 1, false},
    {549, sEffectName_atk_jimen_ougi_DOROKAKE, sEffectName_HEAD, nullptr, 5, false},
    {550, sEffectName_atk_koori_enkaku_YUKINADARE, sEffectName_ROOT, nullptr, 2, false},
    {551, sEffectName_atk_sp_ido_TEREPOOTO, sEffectName_HEAD, nullptr, 0, false},
    {552, sEffectName_atk_battle_kinsetu_RIBENZI, sEffectName_HEAD, nullptr, 3, false},
    {553, sEffectName_atk_normal_ougi_NAKIGOE, sEffectName_HEAD, nullptr, 5, false},
    {554, sEffectName_atk_normal_ougi_IYANAOTO, sEffectName_HEAD, nullptr, 5, false},
    {555, sEffectName_atk_metal_shoot_MIRAASYOTTO, sEffectName_HEAD, nullptr, 0, false},
    {556, sEffectName_atk_jimen_shoot_DOROBAKUDAN, sEffectName_HEAD, nullptr, 0, false},
    {557, sEffectName_atk_aku_tyuusin_WARUDAKUMI, sEffectName_ROOT, nullptr, 1, false},
    {558, sEffectName_atk_iwa_tyuusin_ROKKUKATTO, sEffectName_ROOT, nullptr, 1, false},
    {559, sEffectName_atk_water_tyuusin_KARANIKOMORU, sEffectName_ROOT, nullptr, 1, false},
    {560, sEffectName_atk_water_tyuusin_AKUARINGU, sEffectName_ROOT, nullptr, 1, false},
    {561, sEffectName_atk_sp_tyuusin_KOSUMOPAWAA, sEffectName_ROOT, nullptr, 1, false},
    {562, sEffectName_atk_sp_tyuusin_DOWASURE, sEffectName_ROOT, nullptr, 1, false},
    {563, sEffectName_atk_sp_tyuusin_BARIAA, sEffectName_ROOT, nullptr, 1, false},
    {564, sEffectName_atk_sp_tyuusin_ZYUURYOKU, sEffectName_ROOT, nullptr, 1, false},
    {565, sEffectName_atk_sp_tyuusin_MIRAAKOOTO, sEffectName_ROOT, nullptr, 1, false},
    {566, sEffectName_atk_sp_tyuusin_KOUSOKUIDOU, sEffectName_ROOT, nullptr, 1, false},
    {567, sEffectName_atk_sp_tyuusin_HIKARINOKABE, sEffectName_ROOT, nullptr, 1, false},
    {568, sEffectName_atk_sp_tyuusin_RIHUREKUTAA, sEffectName_ROOT, nullptr, 1, false},
    {569, sEffectName_atk_sp_tyuusin_YOGANOPOOZU, sEffectName_ROOT, nullptr, 1, false},
    {570, sEffectName_atk_sp_tyuusin_MEISOU, sEffectName_ROOT, nullptr, 1, false},
    {571, sEffectName_atk_metal_tyuusin_TEPPEKI, sEffectName_ROOT, nullptr, 1, false},
    {572, sEffectName_atk_metal_tyuusin_METARUBAASUTO, sEffectName_ROOT, nullptr, 1, false},
    {573, sEffectName_atk_poison_tyuusin_TOKERU, sEffectName_ROOT, nullptr, 1, false},
    {574, sEffectName_atk_mushi_tyuusin_BOUGYOSIREI, sEffectName_ROOT, nullptr, 1, false},
    {575, sEffectName_atk_mushi_tyuusin_KAIHUKUSIREI, sEffectName_ROOT, nullptr, 1, false},
    {576, sEffectName_atk_mushi_tyuusin_HOTARUBI, sEffectName_ROOT, nullptr, 1, false},
    {577, sEffectName_atk_normal_tyuusin_MARUKUNARU, sEffectName_ROOT, nullptr, 1, false},
    {578, sEffectName_atk_normal_tyuusin_KOKORONOME, sEffectName_ROOT, nullptr, 1, false},
    {579, sEffectName_atk_normal_tyuusin_ROKKUON, sEffectName_ROOT, nullptr, 1, false},
    {580, sEffectName_atk_normal_tyuusin_KORAERU, sEffectName_ROOT, nullptr, 1, false},
    {581, sEffectName_atk_normal_tyuusin_MAMORU, sEffectName_ROOT, nullptr, 1, false},
    {582, sEffectName_atk_normal_tyuusin_IYASINOSUZU, sEffectName_ROOT, nullptr, 1, false},
    {583, sEffectName_atk_normal_tyuusin_RIHURESSYU, sEffectName_ROOT, nullptr, 1, false},
    {584, sEffectName_atk_normal_tyuusin_SINPINOMAMORI, sEffectName_ROOT, nullptr, 1, false},
    {585, sEffectName_atk_normal_tyuusin_ZIKOSAISEI, sEffectName_ROOT, nullptr, 1, false},
    {586, sEffectName_atk_normal_tyuusin_TAMAGOUMI, sEffectName_ROOT, nullptr, 1, false},
    {587, sEffectName_atk_normal_tyuusin_NAMAKERU, sEffectName_ROOT, nullptr, 1, false},
    {588, sEffectName_atk_normal_tyuusin_NEGAIGOTO, sEffectName_ROOT, nullptr, 1, false},
    {589, sEffectName_atk_normal_tyuusin_MIRUKUNOMI, sEffectName_ROOT, nullptr, 1, false},
    {590, sEffectName_atk_normal_tyuusin_NEMURU, sEffectName_ROOT, nullptr, 1, false},
    {591, sEffectName_atk_normal_tyuusin_KAKUBARU, sEffectName_ROOT, nullptr, 1, false},
    {592, sEffectName_atk_normal_tyuusin_SEITYOU, sEffectName_ROOT, nullptr, 1, false},
    {593, sEffectName_atk_normal_tyuusin_TURUGINOMAI, sEffectName_ROOT, nullptr, 1, false},
    {594, sEffectName_atk_normal_tyuusin_TOOBOE, sEffectName_ROOT, nullptr, 1, false},
    {595, sEffectName_atk_normal_tyuusin_KIAIDAME, sEffectName_ROOT, nullptr, 1, false},
    {596, sEffectName_atk_normal_tyuusin_HARADAIKO, sEffectName_ROOT, nullptr, 1, false},
    {597, sEffectName_atk_electric_tyuusin_ZYUUDEN, sEffectName_ROOT, nullptr, 1, false},
    {598, sEffectName_atk_dragon_tyuusin_RYUUNOMAI, sEffectName_ROOT, nullptr, 1, false},
    {599, sEffectName_atk_kusa_tyuusin_AROMASERAPII, sEffectName_ROOT, nullptr, 1, false},
    {600, sEffectName_atk_koori_tyuusin_KUROIKIRI, sEffectName_ROOT, nullptr, 1, false},
    {601, sEffectName_atk_koori_tyuusin_SIROIKIRI, sEffectName_ROOT, nullptr, 1, false},
    {602, sEffectName_atk_battle_tyuusin_MIKIRI, sEffectName_ROOT, nullptr, 1, false},
    {603, sEffectName_atk_battle_tyuusin_KAUNTAA, sEffectName_ROOT, nullptr, 1, false},
    {604, sEffectName_atk_battle_tyuusin_BIRUDOAPPU, sEffectName_ROOT, nullptr, 1, false},
    {605, sEffectName_atk_hikou_tyuusin_FEZAADANSU, sEffectName_ROOT, nullptr, 1, false},
    {606, sEffectName_atk_hikou_tyuusin_OIKAZE, sEffectName_ROOT, nullptr, 1, false},
    {607, sEffectName_atk_hikou_tyuusin_HANEYASUME, sEffectName_ROOT, nullptr, 1, false},
    {608, sEffectName_atk_aku_kinsetu_SIPPEGAESI, sEffectName_HEAD, nullptr, 3, false},
    {609, sEffectName_atk_aku_ougi_ITYAMON, sEffectName_HEAD, nullptr, 5, false},
    {610, sEffectName_atk_battle_kinsetu_GAMUSYARA, sEffectName_HEAD, nullptr, 3, false},
    {611, sEffectName_atk_battle_kinsetu_KIAIPANTI, sEffectName_HEAD, nullptr, 3, false},
    {612, sEffectName_atk_normal_ougi_HEBINIRAMI, sEffectName_HEAD, nullptr, 5, false},
    {613, sEffectName_atk_normal_ougi_KANASIBARI, sEffectName_HEAD, nullptr, 5, false},
    {614, sEffectName_atk_normal_ougi_KOWAIKAO, sEffectName_HEAD, nullptr, 5, false},
    {615, sEffectName_atk_normal_kinsetu_KUSUGURU, sEffectName_HEAD, nullptr, 3, false},
    {616, sEffectName_atk_normal_kinsetu_ITAMIWAKE, sEffectName_HEAD, nullptr, 3, false},
    {617, sEffectName_atk_normal_ougi_ENMAKU, sEffectName_HEAD, nullptr, 5, false},
    {618, sEffectName_atk_normal_ougi_AMAERU, sEffectName_HEAD, nullptr, 5, false},
    {619, sEffectName_atk_normal_tyuusin_SIPPOWOHURU, sEffectName_ROOT, nullptr, 1, false},
    {620, sEffectName_atk_normal_ougi_NIRAMITUKERU, sEffectName_HEAD, nullptr, 5, false},
    {621, sEffectName_atk_normal_ougi_HURASSYU, sEffectName_HEAD, nullptr, 5, false},
    {622, sEffectName_atk_jimen_ougi_SUNAKAKE, sEffectName_HEAD, nullptr, 5, false},
    {623, sEffectName_atk_sp_ougi_SUPUUNMAGE, sEffectName_HEAD, nullptr, 5, false},
    {624, sEffectName_atk_aku_ougi_USONAKI, sEffectName_HEAD, nullptr, 5, false},
    {625, sEffectName_atk_metal_tyuusin_KINZOKUON, sEffectName_ROOT, nullptr, 1, false},
    {626, sEffectName_atk_kusa_ougi_WATAHOUSI, sEffectName_HEAD, nullptr, 5, false},
    {627, sEffectName_atk_kusa_shoot_YADORIGINOTANE, sEffectName_HEAD, nullptr, 0, false},
    {628, sEffectName_atk_mushi_enkaku_KUMONOSU, sEffectName_ROOT, nullptr, 2, false},
    {629, sEffectName_atk_normal_cube_KUROIMANAZASI, sEffectName_HEAD, nullptr, 4, false},
    {630, sEffectName_atk_normal_cube_TOOSENBOU, sEffectName_HEAD, nullptr, 4, false},
    {631, sEffectName_atk_jimen_cube_ZIWARE, sEffectName_HEAD, nullptr, 4, false},
    {632, sEffectName_atk_koori_cube_ZETTAIREIDO, sEffectName_HEAD, nullptr, 4, false},
    {633, sEffectName_atk_normal_ougi_HUKITOBASI, sEffectName_HEAD, nullptr, 5, false},
    {634, sEffectName_atk_normal_ougi_HOERU, sEffectName_HEAD, nullptr, 5, false},
    {635, sEffectName_atk_metal_tyuusin_HAMETUNONEGAI, sEffectName_ROOT, nullptr, 1, false},
    {636, sEffectName_atk_sp_enkaku_MIRAIYOTI, sEffectName_ROOT, nullptr, 2, false},
    {637, sEffectName_atk_normal_ido_TUNODORIRU, sEffectName_HEAD, nullptr, 0, false},
    {638, sEffectName_atk_mushi_ido_TONBOGAERI, sEffectName_HEAD, nullptr, 0, false},
    {639, sEffectName_atk_normal_tyuusin_HOROBINOUTA, sEffectName_ROOT, nullptr, 1, false},
    {640, sEffectName_atk_sp_tyuusin_MAZIKKUKOOTO, sEffectName_ROOT, nullptr, 1, false},
    {641, sEffectName_atk_ghost_tyuusin_MITIDURE, sEffectName_ROOT, nullptr, 1, false},
    {642, sEffectName_atk_aku_kinsetu_DOROBOU, sEffectName_HEAD, nullptr, 3, false},
    {643, sEffectName_atk_aku_kinsetu_HATAKIOTOSU, sEffectName_HEAD, nullptr, 3, false},
    {644, sEffectName_atk_aku_kinsetu_DAMEOSI, sEffectName_HEAD, nullptr, 3, false},
    {645, sEffectName_atk_normal_kinsetu_TUBOWOTUKU, sEffectName_HEAD, nullptr, 3, false},
    {646, sEffectName_atk_normal_kinsetu_TEDASUKE, sEffectName_HEAD, nullptr, 3, false},
    {647, sEffectName_atk_normal_ougi_HASAMIGIROTIN, sEffectName_HEAD, nullptr, 5, false},
    {648, sEffectName_atk_normal_kinsetu_HOSIGARU, sEffectName_HEAD, nullptr, 3, false},
    {649, sEffectName_atk_normal_ougi_HIMITUNOTIKARA, sEffectName_HEAD, nullptr, 5, false},
    {650, sEffectName_atk_mushi_kinsetu_MUSIKUI, sEffectName_HEAD, nullptr, 3, false},
    {651, sEffectName_atk_poison_enkaku_DOKUBISI, sEffectName_ROOT, nullptr, 2, false},
    {652, sEffectName_atk_jimen_enkaku_MAKIBISI, sEffectName_ROOT, nullptr, 2, false},
    {653, sEffectName_atk_normal_shoot_NEKONIKOBAN, sEffectName_HEAD, nullptr, 0, false},
    {654, sEffectName_atk_kusa_enkaku_KOUGOUSEI, sEffectName_ROOT, nullptr, 2, false},
    {655, sEffectName_atk_normal_enkaku_TUKINOHIKARI, sEffectName_ROOT, nullptr, 2, false},
    {656, sEffectName_atk_normal_enkaku_ASANOHIZASI, sEffectName_ROOT, nullptr, 2, false},
    {657, sEffectName_atk_mushi_ougi_ITOWOHAKU, sEffectName_HEAD, nullptr, 5, false},
    {658, sEffectName_atk_normal_tyuusin_TIISAKUNARU, sEffectName_ROOT, nullptr, 1, false},
    {659, sEffectName_atk_ghost_kinsetu_SYADOOPANTI, sEffectName_HEAD, nullptr, 0, false},
    {660, sEffectName_atk_koori_cube_KOORINOTUBUTE, sEffectName_HEAD, nullptr, 4, false},
    {661, sEffectName_atk_fire_ougi_OOBAAHIITO, sEffectName_HEAD, nullptr, 5, false},
    {662, sEffectName_atk_iwa_enkaku_GANSEKIHUUZI, sEffectName_ROOT, nullptr, 2, false},
    {663, sEffectName_atk_mushi_tyuusin_MUSINOSAZAMEKI, sEffectName_ROOT, nullptr, 1, false},
    {664, sEffectName_atk_kusa_tyuusin_SIIDOHUREA, sEffectName_ROOT, nullptr, 1, false},
    {665, sEffectName_atk_aku_enkaku_DAAKUHOORU, sEffectName_ROOT, nullptr, 2, false},
    {666, sEffectName_atk_sp_oneshot_DOWASURE, sEffectName_HEAD, nullptr, 0, false},
    {667, sEffectName_atk_sp_oneshot_BARIAA, sEffectName_HEAD, nullptr, 0, false},
    {668, sEffectName_atk_sp_oneshot_SUPUUNMAGE, sEffectName_HEAD, nullptr, 0, false},
    {669, sEffectName_atk_normal_oneshot_TAMAGOUMI, sEffectName_HEAD, nullptr, 0, false},
    {670, sEffectName_atk_normal_oneshot_MIRUKUNOMI, sEffectName_HEAD, nullptr, 0, false},
    {671, sEffectName_atk_normal_oneshot_KAKUBARU, sEffectName_HEAD, nullptr, 0, false},
    {672, sEffectName_atk_sp_oneshot_HIKARINOKABE, sEffectName_HEAD, nullptr, 0, false},
    {673, sEffectName_atk_sp_oneshot_RIHUREKUTAA, sEffectName_HEAD, nullptr, 0, false},
    {674, sEffectName_atk_sp_oneshot_MAZIKKUKOOTO, sEffectName_HEAD, nullptr, 0, false},
    {675, sEffectName_atk_mushi_oneshot_BOUGYOSIREI, sEffectName_HEAD, nullptr, 0, false},
    {676, sEffectName_atk_iwa_oneshot_ROKKUKATTO, sEffectName_HEAD, nullptr, 0, false},
    {677, sEffectName_atk_mushi_oneshot_KAIHUKUSIREI, sEffectName_HEAD, nullptr, 0, false},
    {678, sEffectName_atk_hikou_oneshot_HANEYASUME, sEffectName_HEAD, nullptr, 0, false},
    {679, sEffectName_atk_normal_oneshot_HARADAIKO, sEffectName_HEAD, nullptr, 0, false},
    {680, sEffectName_atk_battle_oneshot_GAMUSYARA, sEffectName_HEAD, nullptr, 0, false},
    {681, sEffectName_atk_normal_oneshot_ITAMIWAKE, sEffectName_HEAD, nullptr, 0, false},
    {682, sEffectName_atk_normal_oneshot_DAIBAKUHATU, sEffectName_HEAD, nullptr, 0, false},
    {683, sEffectName_atk_normal_oneshot_MARUKUNARU, sEffectName_HEAD, nullptr, 0, false},
    {684, sEffectName_atk_water_oneshot_KARANIKOMORU, sEffectName_HEAD, nullptr, 0, false},
    {685, sEffectName_atk_sp_oneshot_MIRAAKOOTO, sEffectName_HEAD, nullptr, 0, false},
    {686, sEffectName_atk_enkaku_range, sEffectName_ROOT, nullptr, 2, false},
    {687, sEffectName_atk_general_range, sEffectName_ROOT, nullptr, 0, false},
    {688, sEffectName_atk_boss_tyuusin_01, sEffectName_ROOT, nullptr, 1, false},
    {689, sEffectName_atk_boss_ido_01, sEffectName_ROOT, nullptr, 0, false},
    {690, sEffectName_atk_boss_shoot_SAIKOKATTAA, sEffectName_HEAD, nullptr, 0, false},
    {691, sEffectName_walk_boss, sEffectName_ROOT, nullptr, 0, false},
    {692, sEffectName_boss_teisi, sEffectName_HEAD, nullptr, 0, false},
    {693, sEffectName_boss_saikai, sEffectName_HEAD, nullptr, 0, true},
    {694, sEffectName_born_rival_1, sEffectName_ROOT, nullptr, 0, false},
    {695, sEffectName_born_rival_2, sEffectName_ROOT, nullptr, 0, true},
};

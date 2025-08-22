# Inventory Plugin Improvement Tasks

Below is an ordered, actionable checklist for improving the Inventory plugin. Each task is designed to be independently checkable and progresses from high-level architecture to low-level code quality, with Unreal Engine 5.6 best practices in mind.

1. [ ] Define and document the high-level architecture of the Inventory system (InventoryComponent, EquipmentComponent, Items, Fragments, Widgets), including module boundaries and data flow diagrams.
2. [ ] Establish a clear ownership and replication model for UInv_InventoryItem instances (authority-only creation/destruction, subobject replication, lifecycle).
3. [ ] Introduce an explicit interface/protocol for item actions (equip, unequip, consume, drop) to decouple UI from gameplay logic (e.g., via interfaces or command objects).
4. [ ] Audit all server/client boundaries: ensure UI and input code never call server-only functions directly, and validate RPC access on server (HasAuthority checks and guard clauses).
5. [ ] Add thorough validation to all Server RPCs (e.g., Server_DropItem, Server_ConsumeItem, Server_EquipSlotClicked) to prevent invalid/malicious client requests (null checks, ownership, stack bounds, cooldowns).
6. [ ] Ensure NetMulticast RPCs (e.g., Multicast_EquipSlotClicked) do not trust client data; replicate authoritative state changes instead of forwarding client intent when possible.
7. [ ] Review FastArray usage for correctness: mark items/arrays dirty at correct times and guarantee subobject registration/unregistration symmetry (AddRepSubObject and cleanup on removal/destroy).
8. [ ] Implement proper cleanup when removing inventory entries: remove subobjects from replication graph and handle UInv_InventoryItem destruction safely (Finalize/OnDestroyed patterns).
9. [ ] Add replication conditions and relevancy rules for items/equipment to optimize bandwidth (COND_OwnerOnly where applicable; relevancy based on distance/owner).
10. [ ] Create a persistence plan for inventory (save/load): define a deterministic serialization format for UInv_InventoryItem and fragments (including GameplayTags, stack counts, custom data).
11. [ ] Introduce deterministic item IDs or GUIDs for tracking items across replication, save/load, and logs.
12. [ ] Review and document GameplayTag taxonomy (ItemType, EquipmentType, etc.), consolidate tag sources, and add automated validation for tag existence at startup.
13. [ ] Harden equipment flows: ensure UInv_EquipmentComponent removes/destroys actors deterministically on Unequip and handles duplicate equips/unequips gracefully.
14. [ ] Add nullability and lifetime guards around OwningPlayerController, SkeletalMesh, and InventoryComponent in UInv_EquipmentComponent; avoid raw pointer assumptions.
15. [ ] Eliminate temporary debug logging (UE_LOG(LogTemp,...)) or wrap it with a dedicated log category (DECLARE_LOG_CATEGORY) and verbosity levels.
16. [ ] Add structured logs and traces for equip/unequip/drop/consume flows with item IDs and stack deltas to aid debugging.
17. [ ] Introduce error reporting surfaced to UI for user-driven failures (no room, invalid target slot), using existing delegates (e.g., NoRoomInInventory) and new error codes.
18. [ ] Review UInv_SpatialInventory widget for null dereferences on bound widgets (CanvasPanel, Switcher, Grid_* and Buttons), and add defensive checks and early returns.
19. [ ] Remove tick work where possible: evaluate UInv_SpatialInventory::NativeTick and move sporadic logic to events/delegates to reduce per-frame cost.
20. [ ] Ensure OnItemHovered/UnHovered lifecycle consistently shows/hides UInv_ItemDescription with proper timer handling and cancelation on teardown.
21. [ ] Clamp and validate UInv_SpatialInventory::GetTileSize and any size-dependent layout to avoid negative/zero tile sizes.
22. [ ] Add accessibility/UX improvements: keyboard/controller navigation between grids; visual focus highlighting; tooltip delay setting exposed per-user.
23. [ ] Make grid switching (Equippables/Consumables/Craftables/Collectables) data-driven rather than hard-coded; support category registration via data assets.
24. [ ] Add Blueprint-safe wrappers and metadata: ensure all UFUNCTIONs that are expected in Blueprints have correct BlueprintCallable/BlueprintPure/BlueprintAuthorityOnly metadata and proper categories.
25. [ ] Ensure const-correctness across read-only query functions (e.g., HasRoomForItem, CanEquipHoverItem) and mark functions as const where appropriate.
26. [ ] Adopt consistent naming and coding standards per Unreal style (member prefixes, capitalization, private/protected sections, TWeakObjectPtr usage).
27. [ ] Add comprehensive unit tests (where feasible) and functional automation tests for inventory add/stack/consume/drop/equip/unequip flows, including replication scenarios.
28. [ ] Build test utilities to spawn mock items with manifests/fragments for repeatable tests (server and client contexts).
29. [ ] Verify FInv_InventoryFastArray::RemoveEntry(UInv_InventoryItem*) removes and notifies correctly; add missing PreReplicatedRemove/PostReplicatedAdd events for local state updates.
30. [ ] Implement RemoveEntry(UInv_InventoryComponent*) or remove it if not required; avoid dead code and TODOs by replacing with tracked issues.
31. [ ] Ensure HasRoomForItem and stack split logic returns precise FInv_SlotAvailabilityResult with clear error codes for UI.
32. [ ] Review stack manipulation paths for integer overflow/underflow and ensure clamping to allowed min/max per item definition.
33. [ ] Centralize item manifest accessors: avoid exposing mutable manifests in general UI code; use const views or commands to apply changes server-side.
34. [ ] Validate that UInv_InventoryComponent::ToggleInventoryMenu is client-authoritative only for UI and does not cause replicated side effects inadvertently.
35. [ ] Lazy-create and cache InventoryMenu widget safely, ensure removal on component end play, and avoid UMG references persisting after owner destruction.
36. [ ] Add deterministic spawn parameters for dropped items (randomization within configured ranges); validate navmesh reachability and collision before spawn.
37. [ ] Add line-of-sight and safe location checks before SpawnDroppedItem; fallback to inventory if spawn fails.
38. [ ] Add explicit permissions/ownership checks so a client cannot drop or consume items not owned by their pawn.
39. [ ] Introduce data-driven equipment attachment rules (socket names, offsets) with per-equipment-type configuration in fragments or data assets.
40. [ ] Ensure equipped actors are replicated when necessary or spawned only on owning client if purely cosmetic; document the intended behavior.
41. [ ] Guard against duplicate equipped actors per slot; replace/add logic in FindEquippedActorByTag and RemoveEquippedActorByTag to handle multiple matches safely.
42. [ ] Replace MatchesTagExact with broader MatchesTag where appropriate if sub-type equipment tags should match base slots; document chosen semantics.
43. [ ] Verify all bound delegates (AddDynamic) in UInv_EquipmentComponent are unbound in EndPlay/OnComponentDestroyed to prevent dangling references.
44. [ ] Add EndPlay handling for UInv_EquipmentComponent to destroy spawned equipment actors and clean state.
45. [ ] Add thread-safety notes and assumptions; ensure no code is executed from async threads that touches UObjects without marshaling to game thread.
46. [ ] Audit memory ownership for UObjects created in FastArray and ensure no leaks: if items are subobjects, add Outer ownership and let GC handle them; avoid manual delete.
47. [ ] Review and fix any potential nullptr dereferences in UInv_InventoryComponent (OwningController, InventoryMenu, Pawn casts), add IsValid checks.
48. [ ] Add category/visibility filters to inventory UI, including search/filter by tag; ensure performance on large inventories (virtualized lists/grids).
49. [ ] Add analytics hooks (events) for item lifecycle (acquired, consumed, dropped, equipped/unequipped) with opt-in build flags.
50. [ ] Create developer documentation: setup guide, how to define new items/fragments, how to extend equipment types, replication rules, and testing instructions.
51. [ ] Introduce CI checks for coding style (UnrealHeaderTool warnings as errors), run automation tests, and produce packaged artifacts.
52. [ ] Replace magic numbers in SpawnDroppedItem (angles, distances) with named constants and expose ranges via config/data assets; validate ranges at runtime.
53. [ ] Add safeguards around timers (DescriptionTimerHandle) to clear on widget destruction to avoid delayed callbacks to destroyed widgets.
54. [ ] Ensure UI code uses safe weak pointers for cross-component references and checks validity before use each frame.
55. [ ] Implement an inventory capacity and weight system (if in scope) or clearly document that there is no capacity and why.
56. [ ] Add auditing tools for the inventory state (console commands to dump items, equipped actors, and replication status) for debugging.
57. [ ] Ensure all public headers minimize includes and use forward declarations where possible to reduce compile times; include only in .cpp.
58. [ ] Profile and optimize hot paths: adding/removing items, equipping, grid layout; consider pooling of UMG widgets for item tiles.
59. [ ] Ensure all UObject subclasses have appropriate UCLASS/USTRUCT specifiers (BlueprintType, EditInlineNew) aligned with intended use.
60. [ ] Add module unit tests for FInv_InventoryFastArray::NetDeltaSerialize correctness with simulated add/remove/update deltas.
61. [ ] Validate proper use of TWeakObjectPtr storage for grids/slots; ensure lifetime friendliness in editor and PIE transitions.
62. [ ] Harden category switching: DisableButton/SetActiveGrid should prevent double-click issues and restore previous state on failure.
63. [ ] Add clear separation between game data (ItemManifest) and presentation (ItemDescription widget); avoid mutation from UI.
64. [ ] Document security considerations for multiplayer (cheat prevention): server-side inventory validation, authoritative state, and RPC rate limiting.
65. [ ] Add rate limiting or cooldowns on RPC calls like Server_EquipSlotClicked and Server_DropItem to prevent spam.
66. [ ] Create an examples map and sample items demonstrating equipping/unequipping and replication across server/client.
67. [ ] Review and enforce UE 5.6 deprecations and API changes; resolve any warnings from UHT and compiler.
68. [ ] Add Load/Unload hooks for plugin module to register asset types, settings, and console commands.
69. [ ] Add config-driven defaults (INI) for UI timings, drop distances, inventory capacity, and debug toggles.
70. [ ] Ensure Blueprint events/delegates are fired on the correct authority (client/server) and are consistent in single-player and multiplayer.

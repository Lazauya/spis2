scriptName  spisSpecialInventoryWorkbench extends ObjectReference
{Make a workbench reference access items from a designated container in addition to the player's inventory}
import Debug
import spisSpecialInventoryCrafting

ObjectReference Property SourceContainerRef Auto	; Container with the player's crafting materials
GlobalVariable Property spisWorkingAtWorkbench Auto

Event OnActivate ( objectReference triggerRef )
	Debug.Trace("I am " + self)
	spisWorkingAtWorkbench.SetValue(1)
	spisSpecialInventoryCrafting.spisAddItemsFromContainerToContainer(SourceContainerRef, triggerRef, 30)
endEvent

Function Copypasta(ObjectReference akSourceContainer = None, ObjectReference akDestContainer = None, Int aiIndex = 0, Form akForm = None)
	aiIndex = akSourceContainer.GetNumItems()
	While (aiIndex > 0)
		aiIndex -= 1
		akForm = akSourceContainer.GetNthForm(aiIndex)
		akDestContainer.AddItem(akForm, akSourceContainer.GetItemCount(akForm))
	EndWhile
EndFunction

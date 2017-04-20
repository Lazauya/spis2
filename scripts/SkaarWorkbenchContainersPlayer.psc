scriptName  spisWorkbenchContainersPlayer extends Actor
{Add this script to the Player object to remove items from the workbench container while it's being used}

import spisSpecialInventoryCrafting

ObjectReference Property Workbench Auto Hidden			; The workbench that's being used
GlobalVariable Property spisWorkingAtWorkbench Auto	; Set by the workbench when it is activated to indicate that the player is using one of the spis workbenches

Event OnItemRemoved(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akDestContainer)

	If (spisWorkingAtWorkbench.GetValue() == 1)
		ObjectReference SourceContainerRef = Workbench.GetLinkedRef()		; The workbench's source container
		SourceContainerRef.RemoveItem(akBaseItem, aiItemCount, true, None)	; When an item is used during workbench use, also remove it from the workbench's container
	EndIf

EndEvent

Event OnGetUp(ObjectReference akFurniture)
	If(spisWorkingAtWorkbench.GetValue() == 1)
		spisSpecialInventoryCrafting.spisRemoveItemsInContainerFromContainer(Workbench.GetLinkedRef(), self, 30)
		spisWorkingAtWorkbench.SetValue(0)
	EndIf

EndEvent

Event OnSit(ObjectReference akFurniture)	
	Debug.Trace("Asociating furniture " + akFurniture)
	Workbench = akFurniture
EndEvent

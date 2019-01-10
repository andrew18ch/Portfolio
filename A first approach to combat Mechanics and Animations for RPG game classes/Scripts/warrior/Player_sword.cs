using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player_sword : MonoBehaviour {

	public Transform sword,unequip,equip;
	bool equiped_sword;
	Animator anim;

	void Awake(){
	
		anim = GetComponent<Animator>();
	
	}

	void Update(){
		if (Input.GetKeyDown (KeyCode.LeftControl)) {
		
			anim.SetTrigger ("sword_interact");
		
		}
		//sword's position regarding if the payer is armed or not
		if (equiped_sword) {
			sword.position = equip.position;
			sword.rotation = equip.rotation;
		} else {
			sword.position = unequip.position;
			sword.rotation = unequip.rotation;
		}

	

	
	} 

	public void Sword_Equip(){
	   //equip sword
		equiped_sword = true;

	}

	public void Sword_Unequiped(){
	    //unequip sword
		equiped_sword = false;
	
	}

}

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player_shield : MonoBehaviour {

	Animator anim;
	bool equiped;

	// Use this for initialization
	void Awake() {

		anim = GetComponent<Animator> ();
		
	}
	
	// Update is called once per frame
	void Update () {
		//Equip the shield if the player presses the right mouse button and the cooldown variable is greater than 0
		if (Input.GetMouseButton (1) && ( gameObject.GetComponent<Cooldown> ().time > 0 ) ) {
		
			equiped = true;
		
		} else {
		
			equiped = false;
		
		}

		anim.SetBool ("equip",equiped);
		
	}
}

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bow : MonoBehaviour {

	Animator anim;
	bool equiped = false;

	void Awake(){
	
		anim = GetComponent<Animator> ();
	
	}


	// Update is called once per frame
	void Update () {


		if(Input.GetMouseButtonDown(0)){

			if (!equiped) {
				
				anim.SetBool ("equip_bow", true);
				equiped = true;

			}else if((equiped == true) && (anim.GetBool("equip_arrow") == false)){

				anim.SetBool ("unequip_bow",true);
				equiped = false;


			}

		}

		
	}
}

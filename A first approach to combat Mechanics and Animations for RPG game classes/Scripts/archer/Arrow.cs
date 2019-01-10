using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Arrow : MonoBehaviour {

	Animator anim;
	bool arrow_equip = false;
	bool arrow_arm = false;
	public GameObject arrow,attacking_arrow,emit;
	public Transform corr_position, emit_position;
	float speed = 50f;
	int health = 100;
	public Slider healthbar;



	void OnTriggerEnter(Collider other){


		//damage to the character only if he collides with an object which has the tag damage 
		if(other.tag == "En_Weapon"){
			health -= 20;

		}
		if (health <= 0) {

			anim.SetBool ("Death",true);

		}


	}

	void Awake(){
	
		arrow.SetActive (false);
		anim = GetComponent<Animator> ();

	}
	
	// Update is called once per frame
	void Update () {

		emit_position.position = corr_position.position;
		healthbar.value = health;
	   //procedure for equiping an arrow
		if (anim.GetBool ("equip_bow")) {
		   //equiping bow with an arrow
			if(Input.GetMouseButtonDown(1) && (arrow_equip == false) ){
				
				anim.SetBool ("equip_arrow",true);
				arrow_equip = true;
				arrow.SetActive (true);
		    //equip bow's string
			}else if((arrow_equip == true) && Input.GetMouseButtonDown(1) && (arrow_arm == false) ){
				
				anim.SetBool ("arm_arrow",true);
				arrow_arm = true;

			//shoot the arrow with call of propert procedure
			}else if(Input.GetMouseButtonDown(1) && (arrow_arm == true) ){

				arrow.SetActive (false);
				anim.SetBool ("arm_arrow",false);
				anim.SetBool ("equip_arrow",false);
				arrow_arm = false;
				arrow_equip = false;
				if (gameObject.GetComponent<Cooldown> ().time > 0) {
					Shoot ();
				}
			}

		}
		
	}

	void Shoot(){
		
		//shoot the created arrow and destroy it after 5 seconds
		GameObject clone = (GameObject)  Instantiate (attacking_arrow,emit_position.position,emit.transform.rotation);
		Rigidbody rb = clone.GetComponent<Rigidbody> ();
		rb.velocity= emit.transform.TransformDirection(0f,0f,speed);
		gameObject.GetComponent<Cooldown> ().time -= 40;
		Destroy (clone, 5);

	

	}


}

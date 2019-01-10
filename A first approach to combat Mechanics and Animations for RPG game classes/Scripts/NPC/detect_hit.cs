using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class detect_hit : MonoBehaviour {

	Animator anim;

	void OnTriggerEnter(Collider other){
	


		//reduce NPC's health if it collides with an object with specific tags
		if ((other.tag == "Weapon") || (other.tag == "Fireballtag") || (other.tag == "Fire") || (other.tag == "Arrow") ) {
			
			anim.SetBool ("Damage", true);
			gameObject.GetComponent<Chase_final> ().life -= 20;


		}
		//if NPC collides with an object with tag"Slow_motion",reduce animation speed
		if (other.tag == "Slow_motion") {
		
			anim.SetFloat ("VarAnimSpeed",0.2f);
			Invoke ("Reset_speed",5);
		
		}

	
	}

	void OnTriggerStay(Collider other){
	    //if NPC stays on the object with the tag Fire, it continuously lose health
		if (other.tag == "Fire") {
		
			gameObject.GetComponent<Chase_final> ().life -= 2;
			anim.SetBool ("Damage",true);
		
		}


	
	}



	// Use this for initialization
	void Start () {

		anim = 	GetComponent<Animator> ();
		
	}
	
	// Update is called once per frame
	void Update () {

		if (gameObject.GetComponent<Chase_final> ().life <= 0) {

			anim.SetBool ("Death", true);
		}

	}


	void Reset_speed(){
	
		anim.SetFloat ("VarAnimSpeed",1);
	
	}


}

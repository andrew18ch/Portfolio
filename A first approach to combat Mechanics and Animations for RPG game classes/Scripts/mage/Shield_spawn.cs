using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Shield_spawn : MonoBehaviour {

	public Transform player;
	public GameObject bubbleshield;
	private GameObject clonebubble;
	bool CanSpawn = true;
	float nextspawntime;
	Animator anim;

	void Awake(){
	
		anim = GetComponent<Animator> ();
	
	}



	// Update is called once per frame
	void Update () {
		//creation of the shiled if it called and if the player press right mouse button
		if(CanSpawn && Input.GetMouseButtonDown(1)){
			//creation of the shield at player's position
			clonebubble = Instantiate (bubbleshield);
			clonebubble.transform.position = player.position  ;
			anim.SetBool ("equip",true);
			CanSpawn = false;
			nextspawntime = 0.0f;


		}

		if(!CanSpawn){
			//time calculation until next shield spawn
			nextspawntime += Time.deltaTime; 

		}

		if (nextspawntime >= 7) {

			CanSpawn = true;
			anim.SetBool ("equip",false);
			Destroy (clonebubble);

		}
		
	}
}

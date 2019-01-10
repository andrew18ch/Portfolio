using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class mage_attack : MonoBehaviour {

	Animator anim;
	public float tapSpeed = 0.2f;
	private float lastTapTime = -10f;
	private int clickCount = 0;
	public GameObject fireball, fire, explosion,emit;
	public Transform emit_pos,correct_position;
	int health = 100;
	public Slider healthbar;



	void OnTriggerEnter(Collider other){


		//ζημια στο χαρακτηρα μονο αν γινει collide το συγκεκριμενο tagreduce charecter's health if he collides with an object which has the tag "En_Weapon"
		if(other.tag == "En_Weapon"){
			health -= 20;

		}
		if (health <= 0) {

			anim.SetBool ("Death",true);

		}


	}




	void Awake(){

		anim = GetComponent<Animator> ();

	}
	
	// Update is called once per frame
	void Update () {

		emit_pos.position = correct_position.position;
	    //call spell method if the variable time is bigger than 0
		if (gameObject.GetComponent<Cooldown> ().time > 0) {
			Spell ();
		}
		healthbar.value = health;


	}


	void Spell(){
	    //use of specific spell depending on the number of left mouse clicks that the player does
		if ((Input.GetMouseButtonDown (0) )  ) 
		{
			anim.SetBool ("attack",true);
			lastTapTime = Time.time;

			if ((Time.time - lastTapTime) < tapSpeed)
			{
				clickCount += 1;

				if(clickCount >= 3)
				{
					RaycastHit hitinfo;

					if(Physics.Raycast(emit.transform.position,emit.transform.forward,out hitinfo)){

						Debug.Log (hitinfo.collider);

						GameObject clone = (GameObject) Instantiate (fire,hitinfo.point,Quaternion.LookRotation(hitinfo.normal));
						clone.transform.localScale = hitinfo.transform.localScale;
						Destroy (clone, 6);



					}
					
					gameObject.GetComponent<Cooldown> ().time -= 60;
					Debug.Log ("three click");
					clickCount = 0;



				}
			}
		}

		if ((Time.time - lastTapTime) > tapSpeed)
		{
			if (clickCount == 2)
			{
				RaycastHit hitinfo;
			
				if(Physics.Raycast(emit.transform.position,emit.transform.forward,out hitinfo)){

					GameObject clone = (GameObject) Instantiate (explosion,hitinfo.point,Quaternion.LookRotation(hitinfo.normal));

					Destroy (clone, 4);



				}
				
				gameObject.GetComponent<Cooldown> ().time -= 40;
				Debug.Log ("double click");
				clickCount = 0;


			}
			else if (clickCount == 1)
			{
			
				RaycastHit hitinfo;

				if(Physics.Raycast(emit.transform.position,emit.transform.forward,out hitinfo)){

					GameObject clone = (GameObject) Instantiate (fireball,hitinfo.point,Quaternion.LookRotation(hitinfo.normal));
					Destroy (clone, 4);



				}
				gameObject.GetComponent<Cooldown> ().time -= 20;
				Debug.Log ("one click");
				clickCount = 0;


			}



			clickCount = 0;
		}


	}


}

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Chase_final : MonoBehaviour {

	public Transform player;
	public Transform head;
	public int life = 100;
	bool pursuing = false;
	Animator anim;

	// Use this for initialization
	void Start () {

		anim = GetComponent<Animator> ();

	}

	// Update is called once per frame
	void Update () {

		if (life <= 0)
			return;
		//player's position that NPC searches for
		Vector3 direction = player.position - this.transform.position;
		direction.y = 0;
		//vision of NPC regarding the head's angle
		float angle = Vector3.Angle(direction,head.up) ;

		//if the NPC has a distance of 10m and angle of 30 degrees from the player execute the following
		if (Vector3.Distance (player.position, this.transform.position) < 10 && (angle < 30 || pursuing ) ) {


			pursuing = true;
			this.transform.rotation = Quaternion.Slerp (this.transform.rotation, Quaternion.LookRotation (direction), 0.1f);

			anim.SetBool ("Idle", false);
			if (direction.magnitude > 3) {

				this.transform.Translate (0, 0, 0.05f);
				anim.SetBool ("Walk", true);
				anim.SetBool ("Attack", false);

			} else {

				anim.SetBool ("Attack", true);
				anim.SetBool ("Walk", false);

			}


		} else {

			anim.SetBool ("Idle",true);
			anim.SetBool ("Attack",false);
			anim.SetBool ("Walk",false);
			pursuing = false;


		}

	}
}

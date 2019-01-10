using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class test : MonoBehaviour {

	Rigidbody rb;
	public	Transform begin_pos;


	void Awake () {

		rb = GetComponent<Rigidbody> ();
		begin_pos.position = rb.position;

	}
	
	// Update is called once per frame
	void FixedUpdate () {
		
		rb.velocity = Vector3.zero;
		rb.position = begin_pos.position;
		
	}
}

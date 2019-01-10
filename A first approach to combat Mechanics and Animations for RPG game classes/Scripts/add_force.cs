using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class add_force : MonoBehaviour {

	Rigidbody rb;
	public float speed = 100f;
	public float range = 1;

	// Use this for initialization
	void Start () {
		//αποκτηση δεδομενων Rigidbody για το αντικειμενο που εφαρμοσαμε το script και εφαρμογη δυναμεωνacquire information about Rigidbody and impose forces
		rb = GetComponent<Rigidbody> ();
		rb.AddForce (transform.up * speed,ForceMode.Impulse);

	}
	
	// Update is called once per frame
	void FixedUpdate () {
		
	}
}

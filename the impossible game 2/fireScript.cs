using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class fireScript : MonoBehaviour
{
    public GameObject fire0;
    public GameObject fire1;
    public GameObject fire2;

    private void Start()
    {
        hideFire();        
    }

    void OnCollisionEnter2D(Collision2D collision)
    {
        fire0.GetComponent<Renderer>().enabled = true;
        fire1.GetComponent<Renderer>().enabled = true;
        fire2.GetComponent<Renderer>().enabled = true;
        ActorScript.dead = true;
    }

    public void hideFire()
    {
        fire0.GetComponent<Renderer>().enabled = false;
        fire1.GetComponent<Renderer>().enabled = false;
        fire2.GetComponent<Renderer>().enabled = false;
    }
}

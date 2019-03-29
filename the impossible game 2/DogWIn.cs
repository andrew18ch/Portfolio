using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DogWIn : MonoBehaviour
{
    private void OnCollisionEnter2D(Collision2D collision)
    {
        if (collision.collider.gameObject.CompareTag("Player"))
        {


            ActorScript.dead = true;

        }
    }
}

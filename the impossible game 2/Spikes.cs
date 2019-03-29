using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class Spikes : MonoBehaviour
{

    public GameObject player;
    public static bool entered = false;
    private Rigidbody2D rb2d;
    

    private SpriteRenderer rend;
    // Start is called before the first frame update
    void Start()
    {
        rb2d = GetComponent<Rigidbody2D>();
        
    }

    // Update is called once per frame
    void Update()
    {
        
        rend = gameObject.GetComponent<SpriteRenderer>();
        float moveHorizontal = Input.GetAxis("Horizontal");
       // Debug.Log(player.transform.position.x);
        if (player.transform.position.x >= -7.50f)
        {
            //Debug.Log("here");
            rb2d.gravityScale = 1.0f;
            rend.sortingOrder = 2;
            //Debug.Log(rend.sortingOrder);

        }
        
    }

    private void OnCollisionEnter2D(Collision2D collision)
    {
        if (collision.collider.gameObject.CompareTag("Player")) {

            
            ActorScript.dead = true;

        }
    }

}

using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class ActorScript : MonoBehaviour
{
    // from https://unity3d.com/learn/tutorials/projects/2d-ufo-tutorial/controlling-player

    private Rigidbody2D rb2d;   //Store a reference to the Rigidbody2D component required to use 2D Physics.
    public Text timer_txt;
    public Canvas canvas;

    public static bool dead = false;
    public float speed;         //Floating point variable to store the player's movement speed.
    public static float timer = 0;
    public bool keep_time = true;

    public static int death_counter = 0;
    public bool facing_right = true;
    public int jump_count = 0;

    // Use this for initialization
    void Start()
    {
        //Get and store a reference to the Rigidbody2D component so that we can access it.
        rb2d = GetComponent<Rigidbody2D>();

        StartTimer();
    }

    void Update()
    {
        if (Input.GetKeyDown("space")) {
            Jump();
        }
    }

    //FixedUpdate is called at a fixed interval and is independent of frame rate. Put physics code here.
    void FixedUpdate()
    {
        //Store the current horizontal input in the float moveHorizontal.
        float moveHorizontal = Input.GetAxis("Horizontal");
        float moveVertical = Input.GetAxis("Vertical");

        Flip(moveHorizontal);
        rb2d.velocity = new Vector2(moveHorizontal* speed, rb2d.velocity.y);

        if (keep_time) {
            timer_txt.text = (Time.time - timer).ToString("0.00") + " seconds";
        }

        if (rb2d.position.y < -4.5f) {
            dead = true;
        }

        if (dead) {
            RepositionPlayer();
        }
    }

    void Jump()
    {
        if (jump_count < 2) {
            jump_count++;

            int jump_force = 185;
            if (jump_count > 0) {
                jump_force -= 25;
            }
            rb2d.AddForce(this.gameObject.transform.up * jump_force);
        }
    }

    void Flip(float horizontal)
    {
        if ((horizontal < 0.0f && facing_right ) || (horizontal > 0.0f && !facing_right)) {
            facing_right = !facing_right;
            Vector3 obj_scale = transform.localScale;
            obj_scale.x *= -1;
            transform.localScale = obj_scale;
        }
    }

    void OnCollisionEnter2D(Collision2D collision)
    {
        if (collision.collider.gameObject.layer == 8) {
            jump_count = 0;
        }

        if (collision.collider.gameObject.tag == "Finish") {
            keep_time = false;
        }
    }

    void RepositionPlayer()
    {
        StartCoroutine(SleepFor());
    }

    void StartTimer()
    {
        timer = Time.time;
    }

    IEnumerator SleepFor()
    {
        death_counter++;
        dead = false;

        yield return new WaitForSeconds(1);

        //fireScript.hideFire();

        //rb2d.position = new Vector3(-10.44f, -2.5f, 0.0f);
        SceneManager.LoadScene("Gameover");
    }
}

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Camera_following : MonoBehaviour
{
    public GameObject player;
    private Vector2 vel;
    float smoothX = 0.5f;
    float smoothY = 0.5f;

    void FixedUpdate()
    {
        float pos_x = Mathf.SmoothDamp(transform.position.x, player.transform.position.x, ref vel.x, smoothX);
        float pos_y = Mathf.SmoothDamp(transform.position.y, player.transform.position.y, ref vel.y, smoothY);

        if (pos_y < -3.3f) {
        	pos_y = -3.3f;
        } else if(pos_y > 4.0f) {
        	pos_y = 4.0f;
        }

        if (pos_x < -9.8f) {
        	pos_x = -9.8f;
        } else if (pos_x > 5.5f) {
        	pos_x = 5.5f;
        }

        transform.position = new Vector3(pos_x,pos_y,player.transform.position.z - 1.0f);
    }
}

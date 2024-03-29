package de.all4one_racingteam.all4onecontrol

import android.app.Fragment
import android.os.Bundle
import android.util.Log
import android.view.*
import android.widget.RelativeLayout
import kotlinx.android.synthetic.main.drive_layout.*
import org.jetbrains.anko.centerInParent
import org.jetbrains.anko.db.FloatParser
import java.nio.ByteBuffer
import java.nio.ByteOrder
import kotlin.math.atan2
import kotlin.math.roundToInt

class DriveFragment : ControlFragment() {

    override fun getTitle(): String {
        return getString(R.string.nav_drive_name)
    }

    override fun sendValues(relativeX: Float, relativeY: Float) {

        var angle : Float = relativeX * Math.PI.toFloat() / 2
        var speed : Float = relativeY

        Log.d("Drive Fragment", "angle: $angle; speed: $speed")

        var byteBuffer = ByteBuffer.allocate(2 * 4)
        byteBuffer.order(ByteOrder.LITTLE_ENDIAN)
        byteBuffer.putFloat(angle)
        byteBuffer.putFloat(speed)

        var message: ByteArray = byteArrayOf('s'.toByte(), *byteBuffer.array())

        (activity.applicationContext as GlobalState).sendTcpMessage(message)

    }

}
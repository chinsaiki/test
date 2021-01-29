pipeline {
    agent {
        docker { image "ros:kinetic" }
    }
    stages {
        stage('Build')
        {
            steps {
                sh 'echo "Yolooooooooooooooooooooooooooooooooo !"'
                sh 'make'
                archiveArtifacts artifacts: 'strace', fingerprint: true
            }
        }
    }
}

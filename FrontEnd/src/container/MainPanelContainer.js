import MainPanel from '../component/MainPanel.js';
import { connect } from 'react-redux';
import {
  createNewObiew,
} from '../store/actions';

const mapStateToProps = state => ({
  obiews: state.obiews,
});

const mapDispatchToProps = dispatch => ({
  onCreateNewObiew: obiew => dispatch(createNewObiew(obiew)),
});

export default connect(mapStateToProps, mapDispatchToProps)(MainPanel)
